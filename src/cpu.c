#include "cpu.h"
#include "bus.h"
#include "interrupts.h"
#include "optables.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

void cpu_init(struct cpu *cpu, struct bus *bus) {
  assert(bus != nullptr);
  cpu->bus = bus;

  // This skips the bootrom, though it should be emulated at some point
  cpu->PC = 0x100;
  cpu->IR = cpu_read_u8(cpu, cpu->PC++);

  cpu->AF = cpu->BC = cpu->DE = cpu->HL = cpu->SP = 0;
  cpu->IME = cpu->ime_pending = cpu->halt_bug = false;
  cpu->state = CPU_RUNNING;
}

void cpu_step(struct cpu *cpu) {
  // Execute instructions
  switch (cpu->state) {
  case CPU_RUNNING:
    cpu_execute_instruction(cpu, optable_base[cpu_fetch_next_opcode(cpu)]);
    break;
  case CPU_HALTED:
    bus_tick(cpu->bus);
    break;
  case CPU_STOPPED:
    // TODO
    assert(false);
    break;
  }

  // Handle interrupts
  struct interrupts *in = &cpu->bus->interrupts;
  if ((in->IE & in->IF) != 0) {
    cpu->state = CPU_RUNNING;
    if (cpu->IME)
      interrupts_service_pending(in, cpu);
  }
}

uint8_t cpu_fetch_next_opcode(struct cpu *cpu) {
  if (cpu->halt_bug) {
    cpu->halt_bug = false;
    return cpu_read_u8(cpu, cpu->PC);
  } else {
    return cpu_read_u8(cpu, cpu->PC++);
  }
}

void cpu_execute_instruction(struct cpu *cpu, struct instruction instr) {
  if (cpu->ime_pending) {
    cpu->ime_pending = false;
    cpu->IME = true;
  }

  cpu->IR = instr.opcode;
  instr.handler(cpu);
}

void cpu_write_flags(struct cpu *cpu, uint8_t mask, bool val) {
  cpu->F = val ? cpu->F | mask : cpu->F & ~mask;
}

// Logging

void cpu_log_state_reg8(const struct cpu *cpu, FILE *output) {
  fprintf(output,
          "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X "
          "PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
          cpu->A, cpu->F, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L,
          cpu->SP, cpu->PC, bus_read_byte(cpu->bus, cpu->PC),
          bus_read_byte(cpu->bus, cpu->PC + 1),
          bus_read_byte(cpu->bus, cpu->PC + 2),
          bus_read_byte(cpu->bus, cpu->PC + 3));
  fflush(output);
}

void cpu_log_state_reg16(const struct cpu *cpu, FILE *output) {
  fprintf(output,
          "AF:%04X BC:%04X DE:%04X HL:%04X SP:%04X PC:%04X "
          "PCMEM:%02X,%02X,%02X,%02X\n",
          cpu->AF, cpu->BC, cpu->DE, cpu->HL, cpu->SP, cpu->PC,
          bus_read_byte(cpu->bus, cpu->PC),
          bus_read_byte(cpu->bus, cpu->PC + 1),
          bus_read_byte(cpu->bus, cpu->PC + 2),
          bus_read_byte(cpu->bus, cpu->PC + 3));
  fflush(output);
}

// Memory operations

uint8_t cpu_read_u8(const struct cpu *cpu, uint16_t addr) {
  const uint8_t ret = bus_read_byte(cpu->bus, addr);
  bus_tick(cpu->bus);
  return ret;
}

uint16_t cpu_read_u16(const struct cpu *cpu, uint16_t addr) {
  const uint8_t lo = cpu_read_u8(cpu, addr);
  const uint8_t hi = cpu_read_u8(cpu, addr + 1);
  return (uint16_t)hi << 8 | lo;
}

void cpu_write_u8(const struct cpu *cpu, uint16_t addr, uint8_t val) {
  bus_write_byte(cpu->bus, addr, val);
  bus_tick(cpu->bus);
}

void cpu_write_u16(const struct cpu *cpu, uint16_t addr, uint16_t val) {
  cpu_write_u8(cpu, addr, val & 0xFF);
  cpu_write_u8(cpu, addr + 1, val >> 8);
}

void cpu_jump(struct cpu *cpu, uint16_t addr, bool cond) {
  if (cond) {
    cpu->PC = addr;
    bus_tick(cpu->bus);
  }
}

void cpu_jump_rotation(struct cpu *cpu, int8_t offset, bool cond) {
  if (cond) {
    cpu->PC += offset;
    bus_tick(cpu->bus);
  }
}

void cpu_call(struct cpu *cpu, uint16_t addr, bool cond) {
  if (cond) {
    cpu->SP -= 2;
    cpu_write_u16(cpu, cpu->SP, cpu->PC);
    cpu->PC = addr;
    bus_tick(cpu->bus);
  }
}

void cpu_return(struct cpu *cpu, bool cond) {
  if (cond) {
    const uint16_t jmp_addr = cpu_read_u16(cpu, cpu->SP);
    cpu->SP += 2;
    cpu->PC = jmp_addr;
    bus_tick(cpu->bus);
  }
}

// Opcode dispatching

// clang-format off
static uint8_t cpu_get_r8(const struct cpu *cpu, uint8_t idx) {
  switch (idx) {
  case 0: return cpu->B;
  case 1: return cpu->C;
  case 2: return cpu->D;
  case 3: return cpu->E;
  case 4: return cpu->H;
  case 5: return cpu->L;
  case 6: return cpu_read_u8(cpu, cpu->HL);
  case 7: return cpu->A;
  default: unreachable();
  }
}

uint8_t cpu_get_r8_y(const struct cpu *cpu) {
  return cpu_get_r8(cpu, cpu->IR >> 3 & 0x7);
}

uint8_t cpu_get_r8_z(const struct cpu *cpu) {
  return cpu_get_r8(cpu, cpu->IR & 0x7);
}

static void cpu_set_r8(struct cpu *cpu, uint8_t idx, uint8_t val) {
  switch (idx) {
  case 0: cpu->B = val; break;
  case 1: cpu->C = val; break;
  case 2: cpu->D = val; break;
  case 3: cpu->E = val; break;
  case 4: cpu->H = val; break;
  case 5: cpu->L = val; break;
  case 6: cpu_write_u8(cpu, cpu->HL, val); break;
  case 7: cpu->A = val; break;
  default: unreachable();
  }
}

void cpu_set_r8_y(struct cpu *cpu, uint8_t val) {
  cpu_set_r8(cpu, cpu->IR >> 3 & 0x7, val);
}

void cpu_set_r8_z(struct cpu *cpu, uint8_t val) {
  cpu_set_r8(cpu, cpu->IR & 0x7, val);
}

uint16_t cpu_get_r16(const struct cpu *cpu) {
  switch (cpu->IR >> 4 & 0x3) {
  case 0: return cpu->BC;
  case 1: return cpu->DE;
  case 2: return cpu->HL;
  case 3: return cpu->SP;
  default: unreachable();
  }
}

uint16_t cpu_get_r16stk(const struct cpu *cpu) {
  switch (cpu->IR >> 4 & 0x3) {
  case 0: return cpu->BC;
  case 1: return cpu->DE;
  case 2: return cpu->HL;
  case 3: return cpu->AF;
  default: unreachable();
  }
}

uint16_t cpu_get_r16mem(struct cpu *cpu) {
  switch (cpu->IR >> 4 & 0x3) {
  case 0: return cpu->BC;
  case 1: return cpu->DE;
  case 2: return cpu->HL++;
  case 3: return cpu->HL--;
  default: unreachable();
  }
}

void cpu_set_r16(struct cpu *cpu, uint16_t val) {
  switch (cpu->IR >> 4 & 0x3) {
  case 0: cpu->BC = val; break;
  case 1: cpu->DE = val; break;
  case 2: cpu->HL = val; break;
  case 3: cpu->SP = val; break;
  default: unreachable();
  }
}

void cpu_set_r16stk(struct cpu *cpu, uint16_t val) {
  switch (cpu->IR >> 4 & 0x3) {
  case 0: cpu->BC = val; break;
  case 1: cpu->DE = val; break;
  case 2: cpu->HL = val; break;
  case 3: cpu->AF = val & 0xFFF0; break;
  default: unreachable();
  }
}

bool cpu_cc(const struct cpu *cpu) {
  switch (cpu->IR >> 3 & 0x3) {
  case 0: return (cpu->F & FLAG_Z) == 0;
  case 1: return (cpu->F & FLAG_Z) != 0;
  case 2: return (cpu->F & FLAG_C) == 0;
  case 3: return (cpu->F & FLAG_C) != 0;
  default: unreachable();
  }
}
// clang-format on
