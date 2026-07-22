#include "impl_cpu_instrs.h"
#include "bitwise.h"
#include "cpu.h"

/// 8-bit arithmetic implementations

void impl_add(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint16_t sum = A + operand;
  const uint8_t result = (uint8_t)sum;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (A & 0xF) + (operand & 0xF) > 0xF);
  write_bit(&cpu->F, FLAG_C, sum > 0xFF);

  cpu->A = result;
}

void impl_adc(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const bool carry = is_bit_set(cpu->F, FLAG_C);
  const uint16_t sum = A + operand + carry;
  const uint8_t result = (uint8_t)sum;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (A & 0xF) + (operand & 0xF) + carry > 0xF);
  write_bit(&cpu->F, FLAG_C, sum > 0xFF);

  cpu->A = result;
}

void impl_sub(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A - operand;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  set_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (A & 0xF) < (operand & 0xF));
  write_bit(&cpu->F, FLAG_C, A < operand);

  cpu->A = result;
}

void impl_sbc(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const bool carry = is_bit_set(cpu->F, FLAG_C);
  const uint8_t result = A - (operand + carry);

  write_bit(&cpu->F, FLAG_Z, result == 0);
  set_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (A & 0xF) < (operand & 0xF) + carry);
  write_bit(&cpu->F, FLAG_C, A < (uint16_t)(operand + carry));

  cpu->A = result;
}

void impl_cp(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;

  write_bit(&cpu->F, FLAG_Z, A - operand == 0);
  set_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (A & 0xF) < (operand & 0xF));
  write_bit(&cpu->F, FLAG_C, A < operand);
}

uint8_t impl_inc_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand + 1;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (operand & 0xF) == 0xF);

  return result;
}

uint8_t impl_dec_u8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand - 1;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  set_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (operand & 0xF) == 0x0);

  return result;
}

/// 16-bit arithmetic instructions

void impl_add_r16(struct cpu *cpu, uint16_t operand) {
  const uint16_t HL = cpu_get_hl(cpu);
  const uint32_t sum = HL + operand;

  clear_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  write_bit(&cpu->F, FLAG_C, sum > 0xFFFF);

  cpu_set_hl(cpu, sum);
}

uint16_t impl_add_sp_e8(struct cpu *cpu, int8_t e8) {
  const uint16_t SP = cpu->SP;
  const uint16_t sum = SP + e8;

  clear_bit(&cpu->F, FLAG_Z);
  clear_bit(&cpu->F, FLAG_N);
  write_bit(&cpu->F, FLAG_H, (SP & 0xF) + (e8 & 0xF) > 0xF);
  write_bit(&cpu->F, FLAG_C, (SP & 0xFF) + (e8 & 0xFF) > 0xFF);

  return sum;
}

/// Bitwise logic implementations

void impl_and(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A & operand;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  set_bit(&cpu->F, FLAG_H);
  clear_bit(&cpu->F, FLAG_C);

  cpu->A = result;
}

void imple_xor(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A ^ operand;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  clear_bit(&cpu->F, FLAG_C);

  cpu->A = result;
}

void impl_or(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A | operand;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  clear_bit(&cpu->F, FLAG_C);

  cpu->A = result;
}

/// Bit flag implementations

void impl_bit_b3(struct cpu *cpu, uint8_t b3, uint8_t r8) {
  write_bit(&cpu->F, FLAG_Z, !((r8 >> b3) & 1));
  clear_bit(&cpu->F, FLAG_N);
  set_bit(&cpu->F, FLAG_H);
}

/// Bit-shift implementations

uint8_t impl_rl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | is_bit_set(cpu->F, FLAG_C);

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  write_bit(&cpu->F, FLAG_C, operand & 0x80);

  return result;
}

uint8_t impl_rlc(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | ((operand >> 7) & 1);

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  write_bit(&cpu->F, FLAG_C, operand & 0x80);

  return result;
}

uint8_t impl_rr(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (is_bit_set(cpu->F, FLAG_C) << 7) | (operand >> 1);

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  write_bit(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t impl_rrc(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = ((operand & 1) << 7) | (operand >> 1);

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  write_bit(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t impl_sla(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  write_bit(&cpu->F, FLAG_C, operand & 0x80);

  return result;
}

uint8_t impl_sra(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand & 0x80) | (operand >> 1);

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  write_bit(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t impl_srl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand >> 1;

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  write_bit(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t impl_swap(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand << 4) | (operand >> 4);

  write_bit(&cpu->F, FLAG_Z, result == 0);
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  clear_bit(&cpu->F, FLAG_C);

  return result;
}
