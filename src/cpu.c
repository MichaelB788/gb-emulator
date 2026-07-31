#include "cpu.h"
#include "bus.h"
#include "interrupts.h"
#include "optables.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool cpu_init(struct cpu *cpu, struct bus *bus, FILE *log_file) {
  if (!bus) {
    fprintf(stderr, "Invalid bus pointer given to CPU");
    return false;
  }
  cpu->log_file = log_file;

  cpu->bus = bus;

  cpu->A = 0x01;
  cpu->F = 0xB0;
  cpu->B = 0x00;
  cpu->C = 0x13;
  cpu->D = 0x00;
  cpu->E = 0xD8;
  cpu->H = 0x01;
  cpu->L = 0x4D;
  cpu->PC = 0x0100;
  cpu->SP = 0xFFFE;

  cpu->IME = false;
  cpu->ei_called = false;
  cpu->halt_bug = false;

  cpu->state = CPU_RUNNING;
  cpu->IR = cpu_read_byte(cpu, cpu->PC++);
  return true;
}

static void log_instruction(const struct cpu *cpu) {
  if (cpu->log_file) {
    fprintf(cpu->log_file,
            "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X "
            "PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
            cpu->A, cpu->F, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L,
            cpu->SP, cpu->PC, bus_read_byte(cpu->bus, cpu->PC),
            bus_read_byte(cpu->bus, cpu->PC + 1),
            bus_read_byte(cpu->bus, cpu->PC + 2),
            bus_read_byte(cpu->bus, cpu->PC + 3));
    fflush(cpu->log_file);
  }
}

// See: https://gbdev.io/pandocs/Interrupts.html#interrupt-handling
static void service_interrupts(struct cpu *cpu, struct interrupts *interrupts) {
  const uint8_t pending = interrupts->IE & interrupts->IF;
  for (uint8_t i = 0; i < 5; ++i) {
    if ((pending & (1 << i)) != 0) {
      // Two wait states are executed
      bus_tick(cpu->bus);
      bus_tick(cpu->bus);

      // Call the interrupt handler at it's address around 0x40
      cpu_call_a16(cpu, (uint16_t)(0x40 | (i << 3)), true);

      // Interrupt handled
      interrupts->IF &= ~(1 << i);
      cpu->IME = false;
      return;
    }
  }
}

void cpu_step(struct cpu *cpu) {
  switch (cpu->state) {
  case CPU_RUNNING:
    if (cpu->ei_called) {
      cpu->ei_called = false;
      cpu->IME = true;
    }

    cpu->IR = cpu_read_byte(cpu, cpu->PC);
    if (cpu->halt_bug) {
      cpu->halt_bug = false;
    } else {
      ++cpu->PC;
    }

    log_instruction(cpu);
    unprefixed_ins[cpu->IR](cpu);
    break;
  case CPU_HALTED:
    bus_tick(cpu->bus);
    break;
  case CPU_STOPPED:
    // TODO
    break;
  }

  struct interrupts *interrupts = &cpu->bus->interrupt;
  if ((interrupts->IE & interrupts->IF) != 0) {
    cpu->state = CPU_RUNNING;
    if (cpu->IME) {
      service_interrupts(cpu, interrupts);
    }
  }
}

/// Register pair operations

static uint16_t get_regpair(uint8_t hi, uint8_t lo) {
  return (uint16_t)hi << 8 | lo;
}

uint16_t cpu_get_bc(const struct cpu *cpu) {
  return get_regpair(cpu->B, cpu->C);
}

uint16_t cpu_get_de(const struct cpu *cpu) {
  return get_regpair(cpu->D, cpu->E);
}

uint16_t cpu_get_hl(const struct cpu *cpu) {
  return get_regpair(cpu->H, cpu->L);
}

uint16_t cpu_get_af(const struct cpu *cpu) {
  return get_regpair(cpu->A, cpu->F);
}

static void set_regpair(uint8_t *hi, uint8_t *lo, uint16_t val) {
  *hi = val >> 8;
  *lo = val & 0xFF;
}

void cpu_set_bc(struct cpu *cpu, uint16_t val) {
  set_regpair(&cpu->B, &cpu->C, val);
}

void cpu_set_de(struct cpu *cpu, uint16_t val) {
  set_regpair(&cpu->D, &cpu->E, val);
}

void cpu_set_hl(struct cpu *cpu, uint16_t val) {
  set_regpair(&cpu->H, &cpu->L, val);
}

void cpu_set_af(struct cpu *cpu, uint16_t val) {
  cpu->A = val >> 8;
  cpu->F = val & 0xF0;
}

/// Flags operations

void cpu_write_flags(struct cpu *cpu, uint8_t mask, bool val) {
  cpu->F = val ? cpu->F | mask : cpu->F & ~mask;
}

/// Memory operations

uint8_t cpu_read_byte(struct cpu *cpu, uint16_t addr) {
  const uint8_t ret = bus_read_byte(cpu->bus, addr);
  bus_tick(cpu->bus);
  return ret;
}

uint16_t cpu_read_word(struct cpu *cpu, uint16_t addr) {
  const uint8_t lo = cpu_read_byte(cpu, addr);
  const uint8_t hi = cpu_read_byte(cpu, addr + 1);
  return (uint16_t)hi << 8 | lo;
}

void cpu_write_byte(struct cpu *cpu, uint16_t addr, uint8_t val) {
  bus_write_byte(cpu->bus, addr, val);
  bus_tick(cpu->bus);
}

void cpu_write_word(struct cpu *cpu, uint16_t addr, uint16_t val) {
  const uint8_t hi = val >> 8;
  const uint8_t lo = val & 0xFF;
  cpu_write_byte(cpu, addr, lo);
  cpu_write_byte(cpu, addr + 1, hi);
}

void cpu_jump_a16(struct cpu *cpu, uint16_t addr, bool cond) {
  if (cond) {
    cpu->PC = addr;
    bus_tick(cpu->bus); // Internal cycle, possibly when setting PC
  }
}

void cpu_call_a16(struct cpu *cpu, uint16_t addr, bool cond) {
  if (cond) {
    cpu->SP -= 2;
    cpu_write_word(cpu, cpu->SP, cpu->PC);
    cpu->PC = addr;
    bus_tick(cpu->bus); // Internal cycle, possibly when setting PC
  }
}

void cpu_return(struct cpu *cpu, bool cond) {
  if (cond) {
    const uint16_t jmp_addr = cpu_read_word(cpu, cpu->SP);
    cpu->SP += 2;
    cpu->PC = jmp_addr;
    bus_tick(cpu->bus); // Internal cycle, possibly when setting PC
  }
}

/// Opcode dispatching

#define R16_BIT_FIELD(opcode) ((opcode >> 4) & 0x3)

uint16_t cpu_get_r16(const struct cpu *cpu) {
  switch (R16_BIT_FIELD(cpu->IR)) {
  case 0:
    return cpu_get_bc(cpu);
  case 1:
    return cpu_get_de(cpu);
  case 2:
    return cpu_get_hl(cpu);
  case 3:
    return cpu->SP;
  default:
    assert(false && "cpu_get_r16 fail");
  }
}

void cpu_set_r16(struct cpu *cpu, uint16_t val) {
  switch (R16_BIT_FIELD(cpu->IR)) {
  case 0:
    cpu_set_bc(cpu, val);
    break;
  case 1:
    cpu_set_de(cpu, val);
    break;
  case 2:
    cpu_set_hl(cpu, val);
    break;
  case 3:
    cpu->SP = val;
    break;
  default:
    assert(false && "cpu_set_r16 fail");
  }
}

uint16_t cpu_get_r16stk(const struct cpu *cpu) {
  switch (R16_BIT_FIELD(cpu->IR)) {
  case 0:
    return cpu_get_bc(cpu);
  case 1:
    return cpu_get_de(cpu);
  case 2:
    return cpu_get_hl(cpu);
  case 3:
    return cpu_get_af(cpu);
  default:
    assert(false && "cpu_get_r16stk fail");
  }
}

void cpu_set_r16stk(struct cpu *cpu, uint16_t val) {
  switch (R16_BIT_FIELD(cpu->IR)) {
  case 0:
    cpu_set_bc(cpu, val);
    break;
  case 1:
    cpu_set_de(cpu, val);
    break;
  case 2:
    cpu_set_hl(cpu, val);
    break;
  case 3:
    cpu_set_af(cpu, val);
    break;
  default:
    assert(false && "cpu_set_r16stk fail");
  }
}

uint16_t cpu_get_r16mem(struct cpu *cpu) {
  switch (R16_BIT_FIELD(cpu->IR)) {
  case 0:
    return cpu_get_bc(cpu);
  case 1:
    return cpu_get_de(cpu);
  case 2: {
    const uint16_t ret = cpu_get_hl(cpu);
    cpu_set_hl(cpu, ret + 1);
    return ret;
  }
  case 3: {
    const uint16_t ret = cpu_get_hl(cpu);
    cpu_set_hl(cpu, ret - 1);
    return ret;
  }
  default:
    assert(false && "cpu_get_r16mem fail");
  }
}

bool cpu_test_cond(const struct cpu *cpu) {
  switch ((cpu->IR >> 3) & 0x3) {
  case 0:
    return ((cpu->F & FLAG_Z) == 0);
  case 1:
    return ((cpu->F & FLAG_Z) != 0);
  case 2:
    return ((cpu->F & FLAG_C) == 0);
  case 3:
    return ((cpu->F & FLAG_C) != 0);
  default:
    assert(false && "cpu_check_cond fail");
  }
}
