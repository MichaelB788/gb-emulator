#include "cpu_instructions.h"
#include "bus.h"
#include "cpu.h"
#include <stdint.h>
#include <stdio.h>

/// 8-bit arithmetic implementations

void cpu_add_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;

  const uint16_t sum = A + operand;
  const uint8_t result = (uint8_t)sum;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (A & 0xF) + (operand & 0xF) > 0xF);
  cpu_write_flag(cpu, FLAG_C, sum > 0xFF);

  cpu->A = result;
}

void cpu_adc_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const bool carry = (cpu->F & FLAG_C) != 0;

  const uint16_t sum = A + operand + carry;
  const uint8_t result = (uint8_t)sum;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (A & 0xF) + (operand & 0xF) + carry > 0xF);
  cpu_write_flag(cpu, FLAG_C, sum > 0xFF);

  cpu->A = result;
}

void cpu_sub_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A - operand;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F |= FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF));
  cpu_write_flag(cpu, FLAG_C, A < operand);

  cpu->A = result;
}

void cpu_sbc_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const bool carry = (cpu->F & FLAG_C) != 0;
  const uint8_t result = A - (operand + carry);

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F |= FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF) + carry);
  cpu_write_flag(cpu, FLAG_C, A < (uint16_t)(operand + carry));

  cpu->A = result;
}

void cpu_cp_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;

  cpu_write_flag(cpu, FLAG_Z, A - operand == 0);
  cpu->F |= FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF));
  cpu_write_flag(cpu, FLAG_C, A < operand);
}

uint8_t cpu_inc_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand + 1;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (operand & 0xF) == 0xF);

  return result;
}

uint8_t cpu_dec_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand - 1;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F |= FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (operand & 0xF) == 0x0);

  return result;
}

/// 16-bit arithmetic instructions

void cpu_add_r16(struct cpu *cpu, uint16_t operand) {
  const uint16_t HL = cpu->HL;
  const uint32_t sum = HL + operand;

  cpu->F &= ~FLAG_N;
  cpu_write_flag(cpu, FLAG_H, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  cpu_write_flag(cpu, FLAG_C, sum > 0xFFFF);

  cpu->HL = sum;
}

uint16_t cpu_add_sp_e8(struct cpu *cpu, int8_t e8) {
  const uint16_t SP = cpu->SP;
  const uint16_t sum = SP + e8;

  cpu->F &= ~(FLAG_Z | FLAG_N);
  cpu_write_flag(cpu, FLAG_H, (SP & 0xF) + (e8 & 0xF) > 0xF);
  cpu_write_flag(cpu, FLAG_C, (SP & 0xFF) + (e8 & 0xFF) > 0xFF);

  return sum;
}

/// Bitwise logic implementations

void cpu_and_u8(struct cpu *cpu, uint8_t operand) {
  cpu->A &= operand;

  cpu_write_flag(cpu, FLAG_Z, cpu->A == 0);
  cpu->F &= ~(FLAG_N | FLAG_C);
  cpu->F |= FLAG_H;
}

void cpu_xor_u8(struct cpu *cpu, uint8_t operand) {
  cpu->A ^= operand;

  cpu_write_flag(cpu, FLAG_Z, cpu->A == 0);
  cpu->F &= ~(FLAG_N | FLAG_H | FLAG_C);
}

void cpu_or_u8(struct cpu *cpu, uint8_t operand) {
  cpu->A |= operand;

  cpu_write_flag(cpu, FLAG_Z, cpu->A == 0);
  cpu->F &= ~(FLAG_N | FLAG_H | FLAG_C);
}

/// Bit flag implementations

void cpu_bit_b3_u8(struct cpu *cpu, uint8_t b3, uint8_t r8) {
  cpu_write_flag(cpu, FLAG_Z, (r8 & 1 << b3) == 0);
  cpu->F &= ~FLAG_N;
  cpu->F |= FLAG_H;
}

/// Bit-shift implementations

uint8_t cpu_rl_u8(struct cpu *cpu, uint8_t operand) {
  const bool carry = (cpu->F & FLAG_C) != 0;
  const uint8_t result = operand << 1 | carry;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu_write_flag(cpu, FLAG_C, (operand & 0x80) != 0);

  return result;
}

uint8_t cpu_rlc_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | ((operand >> 7) & 1);

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu_write_flag(cpu, FLAG_C, (operand & 0x80) != 0);

  return result;
}

uint8_t cpu_rr_u8(struct cpu *cpu, uint8_t operand) {
  const bool carry = (cpu->F & FLAG_C) != 0;
  const uint8_t result = (carry << 7) | (operand >> 1);

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu_write_flag(cpu, FLAG_C, (operand & 1) != 0);

  return result;
}

uint8_t cpu_rrc_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = ((operand & 1) << 7) | (operand >> 1);

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu_write_flag(cpu, FLAG_C, (operand & 1) != 0);

  return result;
}

uint8_t cpu_sla_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu_write_flag(cpu, FLAG_C, (operand & 0x80) != 0);

  return result;
}

uint8_t cpu_sra_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand & 0x80) | (operand >> 1);

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu_write_flag(cpu, FLAG_C, (operand & 1) != 0);

  return result;
}

uint8_t cpu_srl_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand >> 1;

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu_write_flag(cpu, FLAG_C, (operand & 1) != 0);

  return result;
}

uint8_t cpu_swap_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand << 4) | (operand >> 4);

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~(FLAG_N | FLAG_H | FLAG_C);

  return result;
}

void cpu_daa(struct cpu *cpu) {
  const uint8_t A = cpu->A, F = cpu->F;
  uint8_t result = 0, adjustment = 0;

  if ((F & FLAG_N) != 0) {
    if ((F & FLAG_H) != 0) {
      adjustment |= 0x6;
    }
    if ((F & FLAG_C) != 0) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    if ((F & FLAG_H) != 0 || (A & 0xF) > 0x9) {
      adjustment |= 0x6;
    }
    if ((F & FLAG_C) != 0 || A > 0x99) {
      adjustment |= 0x60;
      cpu->F |= FLAG_C;
    }
    result = A + adjustment;
  }

  cpu_write_flag(cpu, FLAG_Z, result == 0);
  cpu->F &= ~FLAG_H;

  cpu->A = result;
}

void cpu_halt(struct cpu *cpu) {
  cpu->state = !cpu->IME && (cpu->bus->IF & cpu->bus->IE) != 0 ? CPU_HALT_BUG
                                                               : CPU_HALTED;
}

void cpu_illegal(struct cpu *cpu, uint8_t opcode) {
  fprintf(stderr, "Illegal instruction: 0x%02X\n", opcode);
}
