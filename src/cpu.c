#include "cpu.h"
#include "bitwise.h"
#include "bus.h"
#include "cpu_instrs.h"
#include "interrupts.h"
#include "optables.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

bool cpu_init(struct cpu *cpu, struct bus *bus) {
  if (!bus) {
    fprintf(stderr, "Invalid bus pointer given to CPU");
    return false;
  }
  cpu->bus = bus;
  cpu->state = CPU_RUNNING;
  cpu->opcode = 0;

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
  cpu->ime_pending = false;
  return true;
}

uint8_t cpu_tick(struct cpu *cpu) {
  uint8_t cycles = 0;
  if (cpu->state != CPU_HALTED) {
    if (cpu->ime_pending) {
      cpu->IME = true;
      cpu->ime_pending = false;
    }

    cpu->opcode = cpu_fetch_n8(cpu);
    cycles += unprefixed_ins[cpu->opcode](cpu);
  } else {
    cycles += 4;
  }

  struct interrupts *interrupt = &cpu->bus->interrupt;
  if (interrupt->enable & interrupt->flag) {
    cpu->state = CPU_RUNNING;
    if (cpu->IME) {
      cycles += cpu_service_interrupts(cpu, interrupt);
    }
  }

  return cycles;
}

uint8_t cpu_service_interrupts(struct cpu *cpu, struct interrupts *interrupt) {
  const uint8_t pending_interrupts = interrupt->enable & interrupt->flag;
  for (uint8_t i = 0; i < 5; ++i) {
    if (is_bit_set(pending_interrupts, i)) {
      cpu_push_n16(cpu, cpu->PC);
      cpu->PC = 0x40 | (i << 3);

      clear_bit(&interrupt->flag, i);
      cpu->IME = false;
      cpu->ime_pending = false;
      return 20;
    }
  }
  return 0;
}

/// Register pair operations

uint16_t get_regpair(uint8_t hi, uint8_t lo) { return (uint16_t)hi << 8 | lo; }

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

void set_regpair(uint8_t *hi, uint8_t *lo, uint16_t val) {
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

/// Memory operations

uint8_t cpu_fetch_n8(struct cpu *cpu) {
  return bus_read_byte(cpu->bus, cpu->PC++);
}

uint16_t cpu_fetch_n16(struct cpu *cpu) {
  const uint8_t lo = bus_read_byte(cpu->bus, cpu->PC++);
  const uint8_t hi = bus_read_byte(cpu->bus, cpu->PC++);
  return (uint16_t)hi << 8 | lo;
}

uint8_t cpu_read_hl(struct cpu *cpu) {
  return bus_read_byte(cpu->bus, cpu_get_hl(cpu));
}

void cpu_write_hl(struct cpu *cpu, uint8_t val) {
  bus_write_byte(cpu->bus, cpu_get_hl(cpu), val);
}

void cpu_push_n16(struct cpu *cpu, uint16_t val) {
  bus_write_byte(cpu->bus, --cpu->SP, val >> 8);
  bus_write_byte(cpu->bus, --cpu->SP, val & 0xFF);
}

uint16_t cpu_pop_n16(struct cpu *cpu) {
  const uint8_t lo = bus_read_byte(cpu->bus, cpu->SP++);
  const uint8_t hi = bus_read_byte(cpu->bus, cpu->SP++);
  return (uint16_t)hi << 8 | lo;
}

/// Opcode dispatching

#define R16_BIT_FIELD(opcode) (opcode >> 4) & 0x3

uint16_t cpu_get_r16(const struct cpu *cpu) {
  switch (R16_BIT_FIELD(cpu->opcode)) {
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
  switch (R16_BIT_FIELD(cpu->opcode)) {
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
  switch (R16_BIT_FIELD(cpu->opcode)) {
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
  switch (R16_BIT_FIELD(cpu->opcode)) {
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
  switch (R16_BIT_FIELD(cpu->opcode)) {
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

bool cpu_test_cond(struct cpu *cpu) {
  switch ((cpu->opcode >> 3) & 0x3) {
  case 0:
    return !is_bit_set(cpu->F, FLAG_Z);
  case 1:
    return is_bit_set(cpu->F, FLAG_Z);
  case 2:
    return !is_bit_set(cpu->F, FLAG_C);
  case 3:
    return is_bit_set(cpu->F, FLAG_C);
  default:
    assert(false && "cpu_check_cond fail");
  }
}
