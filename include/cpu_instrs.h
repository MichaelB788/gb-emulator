#pragma once
#include <stdint.h>

struct cpu;

/// Load instructions
void ld_r8_r8(struct cpu *cpu);
void ld_r8_u8(struct cpu *cpu);

void ld_r16_u16(struct cpu *cpu);

void ld_r16_ind_a(struct cpu *cpu);
void ld_u16_ind_a(struct cpu *cpu);

void ldh_u8_ind_a(struct cpu *cpu);
void ldh_c_ind_a(struct cpu *cpu);

void ld_a_r16_ind(struct cpu *cpu);
void ld_a_u16_ind(struct cpu *cpu);

void ldh_a_u8_ind(struct cpu *cpu);
void ldh_a_c_ind(struct cpu *cpu);

/// 8-bit arithmetic instructions
void adc_r8(struct cpu *cpu);
void adc_u8(struct cpu *cpu);

void add_r8(struct cpu *cpu);
void add_u8(struct cpu *cpu);

void sbc_r8(struct cpu *cpu);
void sbc_u8(struct cpu *cpu);

void sub_r8(struct cpu *cpu);
void sub_u8(struct cpu *cpu);

void cp_r8(struct cpu *cpu);
void cp_u8(struct cpu *cpu);

void inc_r8(struct cpu *cpu);
void dec_r8(struct cpu *cpu);

/// 16-bit arithmetic instructions
void add_hl_r16(struct cpu *cpu);
void inc_r16(struct cpu *cpu);
void dec_r16(struct cpu *cpu);

/// Bitwise logic instructions
void and_r8(struct cpu *cpu);
void and_u8(struct cpu *cpu);

void or_r8(struct cpu *cpu);
void or_u8(struct cpu *cpu);

void xor_r8(struct cpu *cpu);
void xor_u8(struct cpu *cpu);

void cpl(struct cpu *cpu);

/// Bit flag instructions
void bit_b3_r8(struct cpu *cpu);

void res_b3_r8(struct cpu *cpu);

void set_b3_r8(struct cpu *cpu);

/// Bit shift instructions
void rl_r8(struct cpu *cpu);
void rla(struct cpu *cpu);

void rlc_r8(struct cpu *cpu);
void rlca(struct cpu *cpu);

void rr_r8(struct cpu *cpu);
void rra(struct cpu *cpu);

void rrc_r8(struct cpu *cpu);
void rrca(struct cpu *cpu);

void sla_r8(struct cpu *cpu);
void sra_r8(struct cpu *cpu);
void srl_r8(struct cpu *cpu);
void swap_r8(struct cpu *cpu);

/// Jumps and subroutine instructions
void call_a16(struct cpu *cpu);
void call_cc_a16(struct cpu *cpu);

void jp_hl(struct cpu *cpu);
void jp_a16(struct cpu *cpu);
void jp_cc_a16(struct cpu *cpu);

void jr_e8(struct cpu *cpu);
void jr_cc_e8(struct cpu *cpu);

void ret_cc(struct cpu *cpu);
void ret(struct cpu *cpu);
void reti(struct cpu *cpu);

void rst_vec(struct cpu *cpu);

/// Carry flag instructions
void ccf(struct cpu *cpu);
void scf(struct cpu *cpu);

/// Stack manipulation instructions
void add_sp_e8(struct cpu *cpu);
void ld_hl_sp_e8(struct cpu *cpu);
void ld_u16_ind_sp(struct cpu *cpu);
void ld_sp_hl(struct cpu *cpu);
void pop_r16stk(struct cpu *cpu);
void push_r16stk(struct cpu *cpu);

/// Interrupt-related instructions
void di(struct cpu *cpu);
void ei(struct cpu *cpu);
void halt(struct cpu *cpu);

/// Misc.
void daa(struct cpu *cpu);
void nop(struct cpu *cpu);
void stop(struct cpu *cpu);
void prefix(struct cpu *cpu);
void illegal(struct cpu *cpu);
