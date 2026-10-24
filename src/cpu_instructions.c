#include "cpu_instructions.h"
#include "bus.h"
#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t cpu_set_flags(bool z, bool n, bool h, bool c) {
  return z << 7 | n << 6 | h << 5 | c << 4;
}

/// 8-bit arithmetic implementations

void cpu_add_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t A = cpu->A;

  const uint16_t sum = A + u8;
  const uint8_t result = (uint8_t)sum;

  cpu->F = cpu_set_flags(result == 0, false, (A & 0xF) + (u8 & 0xF) > 0xF,
                         sum > 0xFF);

  cpu->A = result;
}

void cpu_adc_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t A = cpu->A;
  const bool carry = cpu->F & FLAG_C;

  const uint16_t sum = A + u8 + carry;
  const uint8_t result = (uint8_t)sum;

  cpu->F = cpu_set_flags(result == 0, false,
                         (A & 0xF) + (u8 & 0xF) + carry > 0xF, sum > 0xFF);

  cpu->A = result;
}

void cpu_sub_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t A = cpu->A;
  const uint8_t result = A - u8;

  cpu->F = cpu_set_flags(result == 0, true, (A & 0xF) < (u8 & 0xF), A < u8);

  cpu->A = result;
}

void cpu_sbc_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t A = cpu->A;
  const bool carry = cpu->F & FLAG_C;
  const uint8_t result = A - (u8 + carry);

  cpu->F = cpu_set_flags(result == 0, true, (A & 0xF) < (u8 & 0xF) + carry,
                         A < (uint16_t)(u8 + carry));

  cpu->A = result;
}

void cpu_cp_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t A = cpu->A;

  cpu->F = cpu_set_flags(A - u8 == 0, true, (A & 0xF) < (u8 & 0xF), A < u8);
}

uint8_t cpu_inc_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t result = u8 + 1;

  cpu->F =
      cpu_set_flags(result == 0, false, (u8 & 0xF) == 0xF, cpu->F & FLAG_C);

  return result;
}

uint8_t cpu_dec_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t result = u8 - 1;

  cpu->F = cpu_set_flags(result == 0, true, (u8 & 0xF) == 0x0, cpu->F & FLAG_C);

  return result;
}

void cpu_add_r16(struct cpu *cpu, uint16_t u16) {
  const uint16_t HL = cpu_get_hl(cpu);
  const uint32_t sum = HL + u16;

  cpu->F = cpu_set_flags(cpu->F & FLAG_Z, false,
                         (HL & 0xFFF) + (u16 & 0xFFF) > 0xFFF, sum > 0xFFFF);

  cpu_set_hl(cpu, sum);
}

uint16_t cpu_add_sp_i8(struct cpu *cpu, int8_t i8) {
  const uint16_t SP = cpu->SP;
  const uint16_t sum = SP + i8;

  cpu->F = cpu_set_flags(false, false, (SP & 0xF) + (i8 & 0xF) > 0xF,
                         (SP & 0xFF) + (uint8_t)i8 > 0xFF);

  return sum;
}

/// Bitwise logic implementations

void cpu_and_u8(struct cpu *cpu, uint8_t u8) {
  cpu->F = cpu_set_flags((cpu->A &= u8) == 0, false, true, false);
}

void cpu_xor_u8(struct cpu *cpu, uint8_t u8) {
  cpu->F = cpu_set_flags((cpu->A ^= u8) == 0, false, false, false);
}

void cpu_or_u8(struct cpu *cpu, uint8_t u8) {
  cpu->F = cpu_set_flags((cpu->A |= u8) == 0, false, false, false);
}

/// Bit flag implementations

void cpu_bit_b3_u8(struct cpu *cpu, uint8_t b3, uint8_t u8) {
  cpu->F = cpu_set_flags((u8 >> b3 & 1) == 0, false, true, cpu->F & FLAG_C);
}

/// Bit-shift implementations

uint8_t cpu_rl_u8(struct cpu *cpu, uint8_t u8) {
  const bool carry = cpu->F & FLAG_C;
  const uint8_t result = u8 << 1 | carry;

  cpu->F = cpu_set_flags(result == 0, false, false, (u8 & 0x80) != 0);

  return result;
}

uint8_t cpu_rlc_u8(struct cpu *cpu, uint8_t u8) {
  const bool b7 = u8 >> 7 & 1;
  const uint8_t result = u8 << 1 | b7;

  cpu->F = cpu_set_flags(result == 0, false, false, b7);

  return result;
}

uint8_t cpu_rr_u8(struct cpu *cpu, uint8_t u8) {
  const bool carry = cpu->F & FLAG_C;
  const uint8_t result = carry << 7 | u8 >> 1;

  cpu->F = cpu_set_flags(result == 0, false, false, u8 & 1);

  return result;
}

uint8_t cpu_rrc_u8(struct cpu *cpu, uint8_t u8) {
  const bool b0 = u8 & 1;
  const uint8_t result = b0 << 7 | u8 >> 1;

  cpu->F = cpu_set_flags(result == 0, false, false, b0);

  return result;
}

uint8_t cpu_sla_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t result = u8 << 1;

  cpu->F = cpu_set_flags(result == 0, false, false, u8 & 0x80);

  return result;
}

uint8_t cpu_sra_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t result = (u8 & 0x80) | u8 >> 1;

  cpu->F = cpu_set_flags(result == 0, false, false, u8 & 1);

  return result;
}

uint8_t cpu_srl_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t result = u8 >> 1;

  cpu->F = cpu_set_flags(result == 0, false, false, u8 & 1);

  return result;
}

uint8_t cpu_swap_u8(struct cpu *cpu, uint8_t u8) {
  const uint8_t result = u8 << 4 | u8 >> 4;

  cpu->F = cpu_set_flags(result == 0, false, false, false);

  return result;
}

void cpu_daa(struct cpu *cpu) {
  const uint8_t A = cpu->A, F = cpu->F;
  uint8_t result = 0, adjustment = 0;
  bool carry = cpu->F & FLAG_C;

  if (F & FLAG_N) {
    if (F & FLAG_H)
      adjustment += 0x6;
    if (F & FLAG_C)
      adjustment += 0x60;
    result = A - adjustment;
  } else {
    if ((F & FLAG_H) != 0 || (A & 0xF) > 0x9)
      adjustment += 0x6;
    if ((F & FLAG_C) != 0 || A > 0x99) {
      adjustment += 0x60;
      carry = true;
    }
    result = A + adjustment;
  }

  cpu->F = cpu_set_flags(result == 0, cpu->F & FLAG_N, false, carry);

  cpu->A = result;
}

void cpu_halt(struct cpu *cpu) {
  cpu->state =
      !cpu->IME && (cpu->bus->interrupt.IE & cpu->bus->interrupt.IF) != 0
          ? CPU_HALT_BUG
          : CPU_HALTED;
}

void cpu_illegal(struct cpu *cpu, uint8_t opcode) {
  fprintf(stderr, "Illegal instruction: 0x%02X\n", opcode);
}
