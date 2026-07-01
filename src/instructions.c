#include "instructions.h"
#include "cpu.h"
#include "gameboy.h"
#include "optables.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/// Helper functions

uint8_t field_y(uint8_t opcode) { return (opcode >> 3) & 0b111; }

uint8_t field_z(uint8_t opcode) { return opcode & 0b111; }

uint8_t fetch_n8(struct gameboy *gb) { return bus_read(gb, gb->cpu.PC++); }

uint16_t fetch_n16(struct gameboy *gb) {
  const uint8_t lo = bus_read(gb, gb->cpu.PC++);
  const uint8_t hi = bus_read(gb, gb->cpu.PC++);
  return (uint16_t)hi << 8 | lo;
}

uint8_t read_hl(struct gameboy *gb) { return bus_read(gb, get_hl(&gb->cpu)); }

void write_hl(struct gameboy *gb, uint8_t val) {
  bus_write(gb, get_hl(&gb->cpu), val);
}

uint16_t get_r16(const struct cpu *cpu, uint8_t opcode) {
  const uint8_t r16_idx = field_y(opcode) & 0b110;
  if (r16_idx == 0b110)
    return cpu->SP;
  else
    return get_regpair(cpu, r16_idx);
}

void set_r16(struct cpu *cpu, uint8_t opcode, uint16_t val) {
  const uint8_t r16_idx = field_y(opcode) & 0b110;
  if (r16_idx == 0b110)
    cpu->SP = val;
  else
    set_regpair(cpu, r16_idx, val);
}

uint16_t get_r16_ind(struct cpu *cpu, uint8_t opcode) {
  const uint8_t r16_ind_idx = field_y(opcode) & 0b110;
  if (r16_ind_idx == 0b000 || r16_ind_idx == 0b010) {
    return get_regpair(cpu, r16_ind_idx);
  } else {
    const uint16_t ret = get_hl(cpu);
    if (r16_ind_idx == 0b100)
      set_hl(cpu, ret + 1);
    else
      set_hl(cpu, ret - 1);
    return ret;
  }
}

uint16_t get_r16stk(const struct cpu *cpu, uint8_t opcode) {
  const uint8_t r16_stk_idx = field_y(opcode) & 0b110;
  if (r16_stk_idx == 0b110)
    return (uint16_t)cpu->A << 8 | cpu->F;
  else
    return get_regpair(cpu, r16_stk_idx);
}

void set_r16stk(struct cpu *cpu, uint8_t opcode, uint16_t val) {
  const uint8_t r16_stk_idx = field_y(opcode) & 0b110;
  if (r16_stk_idx == 0b110) {
    cpu->A = val >> 8;
    cpu->F = val & 0xF0;
  } else {
    set_regpair(cpu, r16_stk_idx, val);
  }
}

bool check_condition(struct cpu *cpu, enum condition_code cond) {
  switch (cond) {
  case COND_NZ:
    return !is_flag_set(cpu->F, FLAG_Z);
  case COND_Z:
    return is_flag_set(cpu->F, FLAG_Z);
  case COND_NC:
    return !get_carry(cpu->F);
  case COND_C:
    return get_carry(cpu->F);
  }
}

void push_n16(struct gameboy *gb, uint16_t val) {
  bus_write(gb, --gb->cpu.SP, val >> 8);
  bus_write(gb, --gb->cpu.SP, val & 0xFF);
}

uint16_t pop_n16(struct gameboy *gb) {
  const uint8_t lo = bus_read(gb, gb->cpu.SP++);
  const uint8_t hi = bus_read(gb, gb->cpu.SP++);
  return (uint16_t)hi << 8 | lo;
}

/// 8-bit arithmetic implementations

void add_impl(struct cpu *cpu, uint8_t operand, bool carry) {
  const uint8_t A = cpu->A;

  const uint16_t sum = A + operand + carry;
  const uint8_t result = (uint8_t)sum;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, (A & 0xF) + (operand & 0xF) + carry > 0xF);
  set_flag(&cpu->F, FLAG_C, sum > 0xFF);

  cpu->A = result;
}

void sub_impl(struct cpu *cpu, uint8_t operand, bool carry) {
  const uint8_t A = cpu->A;

  const uint8_t result = A - (operand + carry);

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, true);
  set_flag(&cpu->F, FLAG_H, (A & 0xF) < (operand & 0xF) + carry);
  set_flag(&cpu->F, FLAG_C, A < (uint16_t)(operand + carry));

  cpu->A = result;
}

void cp_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;

  set_flag(&cpu->F, FLAG_Z, A - operand == 0);
  set_flag(&cpu->F, FLAG_N, true);
  set_flag(&cpu->F, FLAG_H, (A & 0xF) < (operand & 0xF));
  set_flag(&cpu->F, FLAG_C, A < operand);
}

uint8_t inc_n8_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand + 1;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, (operand & 0xF) == 0xF);

  return result;
}

uint8_t dec_n8_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand - 1;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, true);
  set_flag(&cpu->F, FLAG_H, (operand & 0xF) == 0x0);

  return result;
}

/// 16-bit arithmetic instructions

void add_r16_impl(struct cpu *cpu, uint16_t operand) {
  const uint16_t HL = get_hl(cpu);
  const uint32_t sum = HL + operand;

  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  set_flag(&cpu->F, FLAG_C, sum > 0xFFFF);

  set_hl(cpu, sum);
}

uint16_t add_sp_e8_impl(struct cpu *cpu, int8_t e8) {
  const uint16_t SP = cpu->SP;
  const uint16_t sum = SP + e8;

  set_flag(&cpu->F, FLAG_Z, false);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, (SP & 0xF) + (e8 & 0xF) > 0xF);
  set_flag(&cpu->F, FLAG_C, (SP & 0xFF) + (e8 & 0xFF) > 0xFF);

  return sum;
}

/// Bitwise logic implementations

void and_impl(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A & operand;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, true);
  set_flag(&cpu->F, FLAG_C, false);

  cpu->A = result;
}

void xor_impl(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A ^ operand;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, false);

  cpu->A = result;
}

void or_impl(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;
  const uint8_t result = A | operand;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, false);

  cpu->A = result;
}

/// Bit flag implementations

void bit_b3_impl(struct cpu *cpu, uint8_t b3, uint8_t r8) {
  set_flag(&cpu->F, FLAG_Z, !((r8 >> b3) & 1));
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, true);
}

/// Bit-shift implementations

uint8_t rl_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | get_carry(cpu->F);

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, operand & 0x80);

  return result;
}

uint8_t rlc_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | ((operand >> 7) & 1);

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, operand & 0x80);

  return result;
}

uint8_t rr_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (get_carry(cpu->F) << 7) | (operand >> 1);

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t rrc_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = ((operand & 1) << 7) | (operand >> 1);

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t sla_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, operand & 0x80);

  return result;
}

uint8_t sra_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand & 0x80) | (operand >> 1);

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t srl_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand >> 1;

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, operand & 1);

  return result;
}

uint8_t swap_impl(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand << 4) | (operand >> 4);

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_N, false);
  set_flag(&cpu->F, FLAG_H, false);
  set_flag(&cpu->F, FLAG_C, false);

  return result;
}

/// DAA implementation

void daa_impl(struct cpu *cpu) {
  const uint8_t A = cpu->A;
  const uint8_t F = cpu->F;

  uint8_t result = 0;
  uint8_t adjustment = 0;

  if (is_flag_set(F, FLAG_N)) {
    if (is_flag_set(F, FLAG_H)) {
      adjustment |= 0x6;
    }
    if (get_carry(F)) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    if (is_flag_set(F, FLAG_H) || (A & 0xF) > 0x9) {
      adjustment |= 0x6;
    }
    if (get_carry(F) || A > 0x99) {
      adjustment |= 0x60;
      set_flag(&cpu->F, FLAG_C, true);
    }
    result = A + adjustment;
  }

  set_flag(&cpu->F, FLAG_Z, result == 0);
  set_flag(&cpu->F, FLAG_H, false);

  cpu->A = result;
}

/// Load instructions

int ld_r8_r8(struct gameboy *gb) {
  const uint8_t dest_i = field_y(gb->opcode);
  const uint8_t src_i = field_z(gb->opcode);
  gb->cpu.r8[dest_i] = gb->cpu.r8[src_i];
  return 4;
}

int ld_r8_n8(struct gameboy *gb) {
  const uint8_t dest_i = field_y(gb->opcode);
  gb->cpu.r8[dest_i] = fetch_n8(gb);
  return 8;
}

int ld_r16_n16(struct gameboy *gb) {
  set_r16(&gb->cpu, gb->opcode, fetch_n16(gb));
  return 12;
}

int ld_hl_ind_r8(struct gameboy *gb) {
  const uint8_t src_i = field_z(gb->opcode);
  write_hl(gb, gb->cpu.r8[src_i]);
  return 8;
}

int ld_hl_ind_n8(struct gameboy *gb) {
  write_hl(gb, fetch_n8(gb));
  return 12;
}

int ld_r8_hl_ind(struct gameboy *gb) {
  const uint8_t dest_i = field_y(gb->opcode);
  gb->cpu.r8[dest_i] = read_hl(gb);
  return 8;
}

int ld_r16_ind_a(struct gameboy *gb) {
  bus_write(gb, get_r16_ind(&gb->cpu, gb->opcode), gb->cpu.A);
  return 8;
}

int ld_n16_ind_a(struct gameboy *gb) {
  bus_write(gb, fetch_n16(gb), gb->cpu.A);
  return 16;
}

int ldh_n8_ind_a(struct gameboy *gb) {
  bus_write(gb, 0xFF00 | fetch_n8(gb), gb->cpu.A);
  return 12;
}

int ldh_c_ind_a(struct gameboy *gb) {
  bus_write(gb, 0xFF00 | gb->cpu.C, gb->cpu.A);
  return 8;
}

int ld_a_r16_ind(struct gameboy *gb) {
  gb->cpu.A = bus_read(gb, get_r16_ind(&gb->cpu, gb->opcode));
  return 8;
}

int ld_a_n16_ind(struct gameboy *gb) {
  gb->cpu.A = bus_read(gb, fetch_n16(gb));
  return 16;
}

int ldh_a_n8_ind(struct gameboy *gb) {
  gb->cpu.A = bus_read(gb, 0xFF00 | fetch_n8(gb));
  return 12;
}

int ldh_a_c_ind(struct gameboy *gb) {
  gb->cpu.A = bus_read(gb, 0xFF00 | gb->cpu.C);
  return 8;
}

/// 8-bit arithmetic instructions

int adc_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  add_impl(&gb->cpu, gb->cpu.r8[i], get_carry(gb->cpu.F));
  return 4;
}

int adc_hl_ind(struct gameboy *gb) {
  add_impl(&gb->cpu, read_hl(gb), get_carry(gb->cpu.F));
  return 8;
}

int adc_n8(struct gameboy *gb) {
  add_impl(&gb->cpu, fetch_n8(gb), get_carry(gb->cpu.F));
  return 8;
}

int add_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  add_impl(&gb->cpu, gb->cpu.r8[i], 0);
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
  const uint8_t i = field_z(gb->opcode);
  sub_impl(&gb->cpu, gb->cpu.r8[i], get_carry(gb->cpu.F));
  return 4;
}

int sbc_hl_ind(struct gameboy *gb) {
  sub_impl(&gb->cpu, read_hl(gb), get_carry(gb->cpu.F));
  return 8;
}

int sbc_n8(struct gameboy *gb) {
  sub_impl(&gb->cpu, fetch_n8(gb), get_carry(gb->cpu.F));
  return 8;
}

int sub_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  sub_impl(&gb->cpu, gb->cpu.r8[i], 0);
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
  const uint8_t i = field_z(gb->opcode);
  cp_impl(&gb->cpu, gb->cpu.r8[i]);
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
  const uint8_t i = field_y(gb->opcode);
  gb->cpu.r8[i] = inc_n8_impl(&gb->cpu, gb->cpu.r8[i]);
  return 4;
}

int inc_hl_ind(struct gameboy *gb) {
  write_hl(gb, inc_n8_impl(&gb->cpu, read_hl(gb)));
  return 12;
}

int dec_r8(struct gameboy *gb) {
  const uint8_t i = field_y(gb->opcode);
  gb->cpu.r8[i] = dec_n8_impl(&gb->cpu, gb->cpu.r8[i]);
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

/// Bitwise logic instructions

int and_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  and_impl(&gb->cpu, gb->cpu.r8[i]);
  return 4;
}

int and_hl_ind(struct gameboy *gb) {
  and_impl(&gb->cpu, read_hl(gb));
  return 8;
}

int and_n8(struct gameboy *gb) {
  and_impl(&gb->cpu, fetch_n8(gb));
  return 8;
}

int or_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  or_impl(&gb->cpu, gb->cpu.r8[i]);
  return 4;
}

int or_hl_ind(struct gameboy *gb) {
  or_impl(&gb->cpu, read_hl(gb));
  return 8;
}

int or_n8(struct gameboy *gb) {
  or_impl(&gb->cpu, fetch_n8(gb));
  return 8;
}

int xor_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  xor_impl(&gb->cpu, gb->cpu.r8[i]);
  return 4;
}

int xor_hl_ind(struct gameboy *gb) {
  xor_impl(&gb->cpu, read_hl(gb));
  return 8;
}

int xor_n8(struct gameboy *gb) {
  xor_impl(&gb->cpu, fetch_n8(gb));
  return 8;
}

int cpl(struct gameboy *gb) {
  gb->cpu.A = ~(gb->cpu.A);
  set_flag(&gb->cpu.F, FLAG_N, true);
  set_flag(&gb->cpu.F, FLAG_H, true);
  return 4;
}

/// Bit flag instructions

int bit_b3_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  const uint8_t b3 = field_y(gb->opcode);
  bit_b3_impl(&gb->cpu, b3, gb->cpu.r8[i]);
  return 8;
}

int bit_b3_hl_ind(struct gameboy *gb) {
  const uint8_t b3 = field_y(gb->opcode);
  bit_b3_impl(&gb->cpu, b3, read_hl(gb));
  return 12;
}

int res_b3_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  const uint8_t b3 = field_y(gb->opcode);
  gb->cpu.r8[i] &= ~(1 << b3);
  return 8;
}

int res_b3_hl_ind(struct gameboy *gb) {
  const uint8_t b3 = field_y(gb->opcode);
  write_hl(gb, read_hl(gb) & ~(1 << b3));
  return 16;
}

int set_b3_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  const uint8_t b3 = field_y(gb->opcode);
  gb->cpu.r8[i] |= 1 << b3;
  return 8;
}

int set_b3_hl_ind(struct gameboy *gb) {
  const uint8_t b3 = field_y(gb->opcode);
  write_hl(gb, read_hl(gb) | 1 << b3);
  return 16;
}

/// Bit shfit instructions

int rl_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = rl_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int rl_hl_ind(struct gameboy *gb) {
  write_hl(gb, rl_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

int rla(struct gameboy *gb) {
  gb->cpu.A = rl_impl(&gb->cpu, gb->cpu.A);
  return 4;
}

int rlc_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = rlc_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int rlc_hl_ind(struct gameboy *gb) {
  write_hl(gb, rlc_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

int rlca(struct gameboy *gb) {
  gb->cpu.A = rlc_impl(&gb->cpu, gb->cpu.A);
  return 4;
}

int rr_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = rr_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int rr_hl_ind(struct gameboy *gb) {
  write_hl(gb, rr_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

int rra(struct gameboy *gb) {
  gb->cpu.A = rr_impl(&gb->cpu, gb->cpu.A);
  return 4;
}

int rrc_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = rrc_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int rrc_hl_ind(struct gameboy *gb) {
  write_hl(gb, rrc_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

int rrca(struct gameboy *gb) {
  gb->cpu.A = rrc_impl(&gb->cpu, gb->cpu.A);
  return 4;
}

int sla_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = sla_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int sla_hl_ind(struct gameboy *gb) {
  write_hl(gb, sla_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

int sra_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = sra_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int sra_hl_ind(struct gameboy *gb) {
  write_hl(gb, sra_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

int srl_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = srl_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int srl_hl_ind(struct gameboy *gb) {
  write_hl(gb, srl_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

int swap_r8(struct gameboy *gb) {
  const uint8_t i = field_z(gb->opcode);
  gb->cpu.r8[i] = swap_impl(&gb->cpu, gb->cpu.r8[i]);
  return 8;
}

int swap_hl_ind(struct gameboy *gb) {
  write_hl(gb, swap_impl(&gb->cpu, read_hl(gb)));
  return 16;
}

/// Jumps and subroutine instructions

int call_n16(struct gameboy *gb) {
  const uint16_t jmp_addr = fetch_n16(gb);
  push_n16(gb, gb->cpu.PC);
  gb->cpu.PC = jmp_addr;
  return 24;
}

int call_cc_n16(struct gameboy *gb) {
  const uint16_t jmp_addr = fetch_n16(gb);
  const enum condition_code cc = field_y(gb->opcode) & 0b11;
  if (check_condition(&gb->cpu, cc)) {
    push_n16(gb, gb->cpu.PC);
    gb->cpu.PC = jmp_addr;
    return 24;
  }
  return 12;
}

int jp_hl(struct gameboy *gb) {
  gb->cpu.PC = get_hl(&gb->cpu);
  return 4;
}

int jp_n16(struct gameboy *gb) {
  gb->cpu.PC = fetch_n16(gb);
  return 16;
}

int jp_cc_n16(struct gameboy *gb) {
  const uint16_t jmp_addr = fetch_n16(gb);
  const enum condition_code cc = field_y(gb->opcode) & 0b11;
  if (check_condition(&gb->cpu, cc)) {
    gb->cpu.PC = jmp_addr;
    return 16;
  }
  return 12;
}

int jr_e8(struct gameboy *gb) {
  gb->cpu.PC += (int8_t)fetch_n8(gb);
  return 12;
}

int jr_cc_e8(struct gameboy *gb) {
  const int8_t offset = fetch_n8(gb);
  const enum condition_code cc = field_y(gb->opcode) & 0b11;
  if (check_condition(&gb->cpu, cc)) {
    gb->cpu.PC += offset;
    return 12;
  }
  return 8;
}

int ret_cc(struct gameboy *gb) {
  const enum condition_code cc = field_y(gb->opcode) & 0b11;
  if (check_condition(&gb->cpu, cc)) {
    gb->cpu.PC = pop_n16(gb);
    return 20;
  }
  return 8;
}

int ret(struct gameboy *gb) {
  gb->cpu.PC = pop_n16(gb);
  return 20;
}

int reti(struct gameboy *gb) {
  gb->cpu.PC = pop_n16(gb);
  gb->cpu.IME = true;
  return 20;
}

int rst_vec(struct gameboy *gb) {
  const uint16_t jmp_addr = gb->opcode & 0x38; // This mask isolates the y field
                                               // without shifting it.
  push_n16(gb, gb->cpu.PC);
  gb->cpu.PC = jmp_addr;
  return 16;
}

/// Carry flag instructions

int ccf(struct gameboy *gb) {
  set_flag(&gb->cpu.F, FLAG_N, false);
  set_flag(&gb->cpu.F, FLAG_H, false);
  set_flag(&gb->cpu.F, FLAG_C, ~get_carry(gb->cpu.F));
  return 4;
}

int scf(struct gameboy *gb) {
  set_flag(&gb->cpu.F, FLAG_N, false);
  set_flag(&gb->cpu.F, FLAG_H, false);
  set_flag(&gb->cpu.F, FLAG_C, true);
  return 4;
}

/// Stack manipulation instructions

int add_sp_e8(struct gameboy *gb) {
  gb->cpu.SP = add_sp_e8_impl(&gb->cpu, fetch_n8(gb));
  return 16;
}

int ld_hl_sp_e8(struct gameboy *gb) {
  set_hl(&gb->cpu, add_sp_e8_impl(&gb->cpu, fetch_n8(gb)));
  return 12;
}

int ld_n16_ind_sp(struct gameboy *gb) {
  const uint16_t n16 = fetch_n16(gb);
  bus_write(gb, n16, gb->cpu.SP & 0xFF);
  bus_write(gb, n16 + 1, gb->cpu.SP >> 8);
  return 20;
}

int ld_sp_hl(struct gameboy *gb) {
  gb->cpu.SP = get_hl(&gb->cpu);
  return 8;
}

int pop_r16stk(struct gameboy *gb) {
  set_r16stk(&gb->cpu, gb->opcode, pop_n16(gb));
  return 12;
}

int push_r16stk(struct gameboy *gb) {
  push_n16(gb, get_r16stk(&gb->cpu, gb->opcode));
  return 16;
}

/// Interrupt-related instructions

int di(struct gameboy *gb) {
  gb->cpu.IME = false;
  return 4;
}

int ei(struct gameboy *gb) {
  gb->cpu.IME = true;
  return 4;
}

int halt(struct gameboy *gb) {
  gb->state = GB_HALTED;
  return 4;
}

/// Misc.

int daa(struct gameboy *gb) {
  daa_impl(&gb->cpu);
  return 4;
}

int nop(struct gameboy *gb) { return 4; }

int stop(struct gameboy *gb) {
  fetch_n8(gb);
  gb->state = GB_STOPPED;
  return 4;
}

int prefix(struct gameboy *gb) {
  gb->opcode = fetch_n8(gb);
  return cbprefixed_ins[gb->opcode](gb);
}

int illegal(struct gameboy *gb) {
  fprintf(stderr, "Illegal instruction: 0x%02X\n", gb->opcode);
  gb->state = GB_STOPPED;
  return 0;
}
