#pragma once
#include <stdbool.h>
#include <stdint.h>

struct gameboy;
struct cpu;

/// Helper functions

uint8_t field_y(uint8_t opcode);

uint8_t field_z(uint8_t opcode);

uint8_t fetch_n8(struct gameboy *gb);

uint16_t fetch_n16(struct gameboy *gb);

uint8_t read_hl(struct gameboy *gb);

void write_hl(struct gameboy *gb, uint8_t val);

uint16_t get_r16(const struct cpu *cpu, uint8_t opcode);

void set_r16(struct cpu *cpu, uint8_t opcode, uint16_t val);

uint16_t get_r16_ind(struct cpu *cpu, uint8_t opcode);

enum condition_code { COND_NZ = 0, COND_Z = 1, COND_NC = 2, COND_C = 3 };
bool check_condition(struct cpu *cpu, enum condition_code cond);

void push_n16(struct gameboy *gb, uint16_t val);

uint16_t pop_n16(struct gameboy *gb);

/// 8-bit arithmetic implementations

void add_impl(struct cpu *cpu, uint8_t operand, bool carry);

void sub_impl(struct cpu *cpu, uint8_t operand, bool carry);

void cp_impl(struct cpu *cpu, uint8_t operand);

uint8_t inc_n8_impl(struct cpu *cpu, uint8_t operand);

uint8_t dec_n8_impl(struct cpu *cpu, uint8_t operand);

/// 16-bit arithmetic implementations

void add_r16_impl(struct cpu *cpu, uint16_t operand);

uint16_t add_sp_e8_impl(struct cpu *cpu, int8_t e8);

/// Bitwise logic implementations

void and_impl(struct cpu *cpu, uint8_t operand);

void xor_impl(struct cpu *cpu, uint8_t operand);

void or_impl(struct cpu *cpu, uint8_t operand);

/// Bit flag implementations

void bit_b3_impl(struct cpu *cpu, uint8_t b3, uint8_t r8);

/// Bit-shift implementations

uint8_t rl_impl(struct cpu *cpu, uint8_t operand);

uint8_t rlc_impl(struct cpu *cpu, uint8_t operand);

uint8_t rr_impl(struct cpu *cpu, uint8_t operand);

uint8_t rrc_impl(struct cpu *cpu, uint8_t operand);

uint8_t sla_impl(struct cpu *cpu, uint8_t operand);

uint8_t sra_impl(struct cpu *cpu, uint8_t operand);

uint8_t srl_impl(struct cpu *cpu, uint8_t operand);

uint8_t swap_impl(struct cpu *cpu, uint8_t operand);

/// DAA implementation

void daa_impl(struct cpu *cpu);

/// Load instructions

int ld_r8_r8(struct gameboy *gb);

int ld_r8_n8(struct gameboy *gb);

int ld_r16_n16(struct gameboy *gb);

int ld_hl_ind_r8(struct gameboy *gb);

int ld_hl_ind_n8(struct gameboy *gb);

int ld_r8_hl_ind(struct gameboy *gb);

int ld_r16_ind_a(struct gameboy *gb);

int ld_n16_ind_a(struct gameboy *gb);

int ldh_n8_ind_a(struct gameboy *gb);

int ldh_c_ind_a(struct gameboy *gb);

int ld_a_r16_ind(struct gameboy *gb);

int ld_a_n16_ind(struct gameboy *gb);

int ldh_a_n8_ind(struct gameboy *gb);

int ldh_a_c_ind(struct gameboy *gb);

/// 8-bit arithmetic instructions

int adc_r8(struct gameboy *gb);

int adc_hl_ind(struct gameboy *gb);

int adc_n8(struct gameboy *gb);

int add_r8(struct gameboy *gb);

int add_hl_ind(struct gameboy *gb);

int add_n8(struct gameboy *gb);

int sbc_r8(struct gameboy *gb);

int sbc_hl_ind(struct gameboy *gb);

int sbc_n8(struct gameboy *gb);

int sub_r8(struct gameboy *gb);

int sub_hl_ind(struct gameboy *gb);

int sub_n8(struct gameboy *gb);

int cp_r8(struct gameboy *gb);

int cp_hl_ind(struct gameboy *gb);

int cp_n8(struct gameboy *gb);

int inc_r8(struct gameboy *gb);

int inc_hl_ind(struct gameboy *gb);

int dec_r8(struct gameboy *gb);

int dec_hl_ind(struct gameboy *gb);

/// 16-bit arithmetic instructions

int add_hl_r16(struct gameboy *gb);

int inc_r16(struct gameboy *gb);

int dec_r16(struct gameboy *gb);

/// Bitwise logic instructions

int and_r8(struct gameboy *gb);

int and_hl_ind(struct gameboy *gb);

int and_n8(struct gameboy *gb);

int or_r8(struct gameboy *gb);

int or_hl_ind(struct gameboy *gb);

int or_n8(struct gameboy *gb);

int xor_r8(struct gameboy *gb);

int xor_hl_ind(struct gameboy *gb);

int xor_n8(struct gameboy *gb);

int cpl(struct gameboy *gb);

/// Bit flag instructions

int bit_b3_r8(struct gameboy *gb);

int bit_b3_hl_ind(struct gameboy *gb);

int res_b3_r8(struct gameboy *gb);

int res_b3_hl_ind(struct gameboy *gb);

int set_b3_r8(struct gameboy *gb);

int set_b3_hl_ind(struct gameboy *gb);

/// Bit shfit instructions

int rl_r8(struct gameboy *gb);

int rl_hl_ind(struct gameboy *gb);

int rla(struct gameboy *gb);

int rlc_r8(struct gameboy *gb);

int rlc_hl_ind(struct gameboy *gb);

int rlca(struct gameboy *gb);

int rr_r8(struct gameboy *gb);

int rr_hl_ind(struct gameboy *gb);

int rra(struct gameboy *gb);

int rrc_r8(struct gameboy *gb);

int rrc_hl_ind(struct gameboy *gb);

int rrca(struct gameboy *gb);

int sla_r8(struct gameboy *gb);

int sla_hl_ind(struct gameboy *gb);

int sra_r8(struct gameboy *gb);

int sra_hl_ind(struct gameboy *gb);

int srl_r8(struct gameboy *gb);

int srl_hl_ind(struct gameboy *gb);

int swap_r8(struct gameboy *gb);

int swap_hl_ind(struct gameboy *gb);

/// Jumps and subroutine instructions

int call_n16(struct gameboy *gb);

int call_cc_n16(struct gameboy *gb);

int jp_hl(struct gameboy *gb);

int jp_n16(struct gameboy *gb);

int jp_cc_n16(struct gameboy *gb);

int jr_e8(struct gameboy *gb);

int jr_cc_e8(struct gameboy *gb);

int ret_cc(struct gameboy *gb);

int ret(struct gameboy *gb);

int reti(struct gameboy *gb);

int rst_vec(struct gameboy *gb);

/// Carry flag instructions

int ccf(struct gameboy *gb);

int scf(struct gameboy *gb);

/// Stack manipulation instructions

int add_sp_e8(struct gameboy *gb);

int ld_hl_sp_e8(struct gameboy *gb);

int ld_n16_ind_sp(struct gameboy *gb);

int ld_sp_hl(struct gameboy *gb);

int pop_r16stk(struct gameboy *gb);

int push_r16stk(struct gameboy *gb);

/// Interrupt-related instructions

int di(struct gameboy *gb);

int ei(struct gameboy *gb);

int halt(struct gameboy *gb);

/// Misc.

int daa(struct gameboy *gb);

int nop(struct gameboy *gb);

int stop(struct gameboy *gb);

int prefix(struct gameboy *gb);

int illegal(struct gameboy *gb);
