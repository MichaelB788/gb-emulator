#include "instructions.h"
#include "cpu.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/// Helper functions

uint8_t field_y(uint8_t opcode) { return (opcode >> 3) & 0b111; }

uint8_t field_z(uint8_t opcode) { return opcode & 0b111; }

uint8_t fetch_n8(struct gameboy *gb) { return read_byte(gb, gb->cpu.PC++); }

uint16_t fetch_n16(struct gameboy *gb) {
  const uint8_t lo = read_byte(gb, gb->cpu.PC++);
  const uint8_t hi = read_byte(gb, gb->cpu.PC++);
  return (uint16_t)hi << 8 | lo;
}

uint8_t read_hl(struct gameboy *gb) { return read_byte(gb, get_hl(&gb->cpu)); }

void write_hl(struct gameboy *gb, uint8_t val) {
  write_byte(gb, get_hl(&gb->cpu), val);
}

uint16_t get_r16(const struct cpu *cpu, uint8_t opcode) {
  const uint8_t r16 = field_y(opcode) & 0b110;
  if (r16 == 0b110)
    return cpu->SP;
  else
    return get_regpair(cpu, r16);
}

void set_r16(struct cpu *cpu, uint8_t opcode, uint16_t val) {
  const uint8_t r16 = field_y(opcode) & 0b110;
  if (r16 == 0b110)
    cpu->SP = val;
  else
    set_regpair(cpu, r16, val);
}

uint16_t get_r16_ind(struct cpu *cpu, uint8_t opcode) {
  const uint8_t r16_ind = field_y(opcode) & 0b110;
  if (r16_ind == 0 || r16_ind == 1) {
    return get_regpair(cpu, r16_ind);
  } else {
    const uint16_t ret = get_hl(cpu);
    if (r16_ind == 3)
      set_hl(cpu, ret + 1);
    else
      set_hl(cpu, ret - 1);
    return ret;
  }
}

uint16_t get_r16stk(const struct cpu *cpu, uint8_t opcode) {
  const uint8_t r16 = field_y(opcode) & 0b110;
  if (r16 == 0b110)
    return (uint16_t)cpu->r8[REG_A] << 8 | cpu->r8[REG_F];
  else
    return get_regpair(cpu, r16);
}

void set_r16stk(struct cpu *cpu, uint8_t opcode, uint16_t val) {
  const uint8_t r16 = field_y(opcode) & 0b110;
  if (r16 == 0b110) {
    cpu->r8[REG_A] = val >> 8;
    cpu->r8[REG_F] = val & 0xF0;
  } else {
    set_regpair(cpu, r16, val);
  }
}

bool test_condition(struct cpu *cpu, enum condition_code cond) {
  switch (cond) {
  case COND_NZ:
    return !is_flag_set(cpu, FLAG_Z);
  case COND_Z: // Z
    return is_flag_set(cpu, FLAG_Z);
  case COND_NC: // NC
    return !is_flag_set(cpu, FLAG_C);
  case COND_C: // C
    return is_flag_set(cpu, FLAG_C);
  }
}

void push_onto_stack(struct gameboy *gb, uint16_t val) {
  write_byte(gb, --gb->cpu.SP, val >> 8);
  write_byte(gb, --gb->cpu.SP, val & 0xFF);
}

uint16_t pop_off_stack(struct gameboy *gb) {
  const uint8_t lo = read_byte(gb, gb->cpu.SP++);
  const uint8_t hi = read_byte(gb, gb->cpu.SP++);
  return (uint16_t)hi << 8 | lo;
}

/// Arithmetic implementations

void add_impl(struct cpu *cpu, uint8_t operand, bool carry) {
  const uint8_t A = cpu->r8[REG_A];

  const uint16_t sum = A + operand + carry;
  const uint8_t result = (uint8_t)sum;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (A & 0xF) + (operand & 0xF) + carry > 0xF);
  set_flag(cpu, FLAG_C, sum > 0xFF);

  cpu->r8[REG_A] = result;
}

void sub_impl(struct cpu *cpu, uint8_t operand, bool carry) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A - (operand + carry);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF) + carry);
  set_flag(cpu, FLAG_C, A < (uint16_t)(operand + carry));

  cpu->r8[REG_A] = result;
}

void cp_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  set_flag(cpu, FLAG_Z, A - operand == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF));
  set_flag(cpu, FLAG_C, A < operand);
}

uint8_t inc_n8_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand + 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (operand & 0xF) == 0xF);

  return result;
}

uint8_t dec_n8_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand - 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (operand & 0xF) == 0x0);

  return result;
}

void add_r16_impl(struct cpu *cpu, uint16_t operand) {
  const uint16_t HL = get_regpair(cpu, REG_HL);
  const uint32_t sum = HL + operand;

  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  set_flag(cpu, FLAG_C, sum > 0xFFFF);

  set_regpair(cpu, REG_HL, sum);
}

uint16_t add_sp_e8_impl(struct cpu *cpu, int8_t e8) {
  const uint16_t SP = cpu->SP;
  const uint16_t sum = SP + e8;

  set_flag(cpu, FLAG_Z, false);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (SP & 0xF) + (e8 & 0xF) > 0xF);
  set_flag(cpu, FLAG_C, sum > 0xFF);

  return sum;
}

/// Bitwise implementations

void and_impl(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];
  const uint8_t result = A & operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, true);
  set_flag(cpu, FLAG_C, false);

  cpu->r8[REG_A] = result;
}

void xor_impl(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];
  const uint8_t result = A ^ operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, false);

  cpu->r8[REG_A] = result;
}

void or_impl(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];
  const uint8_t result = A | operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, false);

  cpu->r8[REG_A] = result;
}

void bit_b3_impl(struct cpu *cpu, uint8_t b3, uint8_t r8) {
  set_flag(cpu, FLAG_Z, !((r8 >> b3) & 1));
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, true);
}

/// Bit-shift implementations

uint8_t rl_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | is_flag_set(cpu, FLAG_C);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 0x80);

  return result;
}

uint8_t rlc_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | ((operand >> 7) & 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 0x80);

  return result;
}

uint8_t rr_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (is_flag_set(cpu, FLAG_C) << 7) | (operand >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 1);

  return result;
}

uint8_t rrc_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = ((operand & 1) << 7) | (operand >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 1);

  return result;
}

uint8_t sla_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 0x80);

  return result;
}

uint8_t sra_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand & 0x80) | (operand >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 1);

  return result;
}

uint8_t srl_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand >> 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 1);

  return result;
}

uint8_t swap_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand << 4) | (operand >> 4);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, false);

  return result;
}

/// Load instructions

int ld_r8_r8(struct gameboy *gb) {
  const enum reg8 dest = field_y(gb->opcode);
  const enum reg8 src = field_z(gb->opcode);
  gb->cpu.r8[dest] = gb->cpu.r8[src];
  return 4;
}

int ld_r8_n8(struct gameboy *gb) {
  const enum reg8 dest = field_y(gb->opcode);
  gb->cpu.r8[dest] = fetch_n8(gb);
  return 8;
}

int ld_r16_n16(struct gameboy *gb) {
  set_r16(&gb->cpu, gb->opcode, fetch_n16(gb));
  return 12;
}

int ld_hl_ind_r8(struct gameboy *gb) {
  const enum reg8 dest = field_z(gb->opcode);
  write_hl(gb, gb->cpu.r8[dest]);
  return 8;
}

int ld_hl_ind_n8(struct gameboy *gb) {
  write_hl(gb, fetch_n8(gb));
  return 8;
}

int ld_r8_hl_ind(struct gameboy *gb) {
  const enum reg8 dest = field_z(gb->opcode);
  gb->cpu.r8[dest] = read_hl(gb);
  return 8;
}

int ld_r16_ind_a(struct gameboy *gb) {
  write_byte(gb, get_r16_ind(&gb->cpu, gb->opcode), gb->cpu.r8[REG_A]);
  return 8;
}

int ld_n16_ind_a(struct gameboy *gb) {
  write_byte(gb, fetch_n16(gb), gb->cpu.r8[REG_A]);
  return 16;
}

int ldh_n8_ind_a(struct gameboy *gb) {
  write_byte(gb, 0xFF00 | fetch_n8(gb), gb->cpu.r8[REG_A]);
  return 12;
}

int ldh_c_ind_a(struct gameboy *gb) {
  write_byte(gb, 0xFF00 | gb->cpu.r8[REG_C], gb->cpu.r8[REG_A]);
  return 8;
}

int ld_a_r16_ind(struct gameboy *gb) {
  gb->cpu.r8[REG_A] = read_byte(gb, get_r16_ind(&gb->cpu, gb->opcode));
  return 8;
}

int ld_a_n16_ind(struct gameboy *gb) {
  gb->cpu.r8[REG_A] = read_byte(gb, fetch_n16(gb));
  return 16;
}

int ldh_a_n8_ind(struct gameboy *gb) {
  gb->cpu.r8[REG_A] = read_byte(gb, 0xFF00 | fetch_n8(gb));
  return 12;
}

int ldh_a_c_ind(struct gameboy *gb) {
  gb->cpu.r8[REG_A] = read_byte(gb, 0xFF00 | gb->cpu.r8[REG_C]);
  return 8;
}

/// 8-bit arithmetic instructions

int adc_r8(struct gameboy *gb) {
  const enum reg8 reg = field_z(gb->opcode);
  add_impl(&gb->cpu, gb->cpu.r8[reg], get_carry(&gb->cpu));
  return 4;
}

int adc_hl_ind(struct gameboy *gb) {
  add_impl(&gb->cpu, read_hl(gb), get_carry(&gb->cpu));
  return 8;
}

int adc_n8(struct gameboy *gb) {
  add_impl(&gb->cpu, fetch_n8(gb), get_carry(&gb->cpu));
  return 8;
}

int add_r8(struct gameboy *gb) {
  const enum reg8 reg = field_z(gb->opcode);
  add_impl(&gb->cpu, gb->cpu.r8[reg], 0);
  return 4;
}

int add_hl_ind(struct gameboy *gb) {
  add_impl(&gb->cpu, read_hl(gb), 0);
  return 8;
}

int add_n8(struct gameboy *gb) {
  add_impl(&gb->cpu, fetch_n8(gb), 0);
  return 8;
}

int sbc_r8(struct gameboy *gb) {
  const enum reg8 reg = field_z(gb->opcode);
  sub_impl(&gb->cpu, gb->cpu.r8[reg], get_carry(&gb->cpu));
  return 4;
}

int sbc_hl_ind(struct gameboy *gb) {
  sub_impl(&gb->cpu, read_hl(gb), get_carry(&gb->cpu));
  return 8;
}

int sbc_n8(struct gameboy *gb) {
  sub_impl(&gb->cpu, fetch_n8(gb), get_carry(&gb->cpu));
  return 8;
}

int sub_r8(struct gameboy *gb) {
  const enum reg8 reg = field_z(gb->opcode);
  sub_impl(&gb->cpu, gb->cpu.r8[reg], 0);
  return 4;
}

int sub_hl_ind(struct gameboy *gb) {
  sub_impl(&gb->cpu, read_hl(gb), 0);
  return 8;
}

int sub_n8(struct gameboy *gb) {
  sub_impl(&gb->cpu, fetch_n8(gb), 0);
  return 8;
}

int cp_r8(struct gameboy *gb) {
  const enum reg8 reg = field_z(gb->opcode);
  cp_impl(&gb->cpu, gb->cpu.r8[reg]);
  return 4;
}

int cp_hl_ind(struct gameboy *gb) {
  cp_impl(&gb->cpu, read_hl(gb));
  return 8;
}

int cp_n8(struct gameboy *gb) {
  cp_impl(&gb->cpu, fetch_n8(gb));
  return 8;
}

int inc_r8(struct gameboy *gb) {
  const enum reg8 reg = field_z(gb->opcode);
  gb->cpu.r8[reg] = inc_n8_impl(&gb->cpu, gb->cpu.r8[reg]);
  return 4;
}

int inc_hl_ind(struct gameboy *gb) {
  write_hl(gb, inc_n8_impl(&gb->cpu, read_hl(gb)));
  return 12;
}

int dec_r8(struct gameboy *gb) {
  const enum reg8 reg = field_z(gb->opcode);
  gb->cpu.r8[reg] = dec_n8_impl(&gb->cpu, gb->cpu.r8[reg]);
  return 4;
}

int dec_hl_ind(struct gameboy *gb) {
  write_hl(gb, dec_n8_impl(&gb->cpu, read_hl(gb)));
  return 12;
}

int add_hl_r16(struct gameboy *gb) {
  add_r16_impl(&gb->cpu, get_r16(&gb->cpu, gb->opcode));
  return 8;
}

int inc_r16(struct gameboy *gb) {
  const uint8_t op = gb->opcode;
  set_r16(&gb->cpu, op, get_r16(&gb->cpu, op) + 1);
  return 8;
}

int dec_r16(struct gameboy *gb) {
  const uint8_t op = gb->opcode;
  set_r16(&gb->cpu, op, get_r16(&gb->cpu, op) - 1);
  return 8;
}

/// Jumps and subroutine instructions

int rst_vec(struct gameboy *gb) {
  const uint16_t jmp_addr = gb->opcode & 0x38;
  push_onto_stack(gb, gb->cpu.PC);
  gb->cpu.PC = jmp_addr;
  return 16;
}

/// Stack manipulation instructions

int pop_r16stk(struct gameboy *gb) {
  set_r16stk(&gb->cpu, gb->opcode, pop_off_stack(gb));
  return 12;
}

int push_r16stk(struct gameboy *gb) {
  push_onto_stack(gb, get_r16stk(&gb->cpu, gb->opcode));
  return 16;
}

/// Misc.

int nop(struct gameboy *gb) { return 4; }

int halt(struct gameboy *gb) {
  gb->state = GB_HALTED;
  return 4;
}

int daa(struct gameboy *gb) {
  struct cpu *cpu = &gb->cpu;
  const uint8_t A = cpu->r8[REG_A];
  uint8_t result = 0;

  if (is_flag_set(cpu, FLAG_N)) {
    uint8_t adjustment = 0;
    if (is_flag_set(cpu, FLAG_H)) {
      adjustment |= 0x6;
    }
    if (is_flag_set(cpu, FLAG_C)) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    uint8_t adjustment = 0;
    if (is_flag_set(cpu, FLAG_H) || (A & 0xF) > 0x9) {
      adjustment |= 0x6;
    }
    if (is_flag_set(cpu, FLAG_C) || A > 0x99) {
      adjustment |= 0x60;
      set_flag(cpu, FLAG_C, true);
    }
    result = A + adjustment;
  }

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_H, false);

  cpu->r8[REG_A] = result;
  return 4;
}

int illegal(struct gameboy *gb) {
  fprintf(stderr, "Illegal instruction: 0x%2X\n", gb->opcode);
  gb->state = GB_STOPPED;
  return 0;
}
