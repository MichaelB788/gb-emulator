#pragma once
#include <stdbool.h>
#include <stdint.h>

struct cpu;

/// Load instructions
uint8_t ld_r8_r8(struct cpu *cpu);
uint8_t ld_r8_n8(struct cpu *cpu);

uint8_t ld_r16_n16(struct cpu *cpu);

uint8_t ld_hl_ind_r8(struct cpu *cpu);
uint8_t ld_hl_ind_n8(struct cpu *cpu);

uint8_t ld_r8_hl_ind(struct cpu *cpu);

uint8_t ld_r16_ind_a(struct cpu *cpu);
uint8_t ld_n16_ind_a(struct cpu *cpu);

uint8_t ldh_n8_ind_a(struct cpu *cpu);
uint8_t ldh_c_ind_a(struct cpu *cpu);

uint8_t ld_a_r16_ind(struct cpu *cpu);
uint8_t ld_a_n16_ind(struct cpu *cpu);

uint8_t ldh_a_n8_ind(struct cpu *cpu);
uint8_t ldh_a_c_ind(struct cpu *cpu);

/// 8-bit arithmetic instructions
uint8_t adc_r8(struct cpu *cpu);
uint8_t adc_hl_ind(struct cpu *cpu);
uint8_t adc_n8(struct cpu *cpu);

uint8_t add_r8(struct cpu *cpu);
uint8_t add_hl_ind(struct cpu *cpu);
uint8_t add_n8(struct cpu *cpu);

uint8_t sbc_r8(struct cpu *cpu);
uint8_t sbc_hl_ind(struct cpu *cpu);
uint8_t sbc_n8(struct cpu *cpu);

uint8_t sub_r8(struct cpu *cpu);
uint8_t sub_hl_ind(struct cpu *cpu);
uint8_t sub_n8(struct cpu *cpu);

uint8_t cp_r8(struct cpu *cpu);
uint8_t cp_hl_ind(struct cpu *cpu);
uint8_t cp_n8(struct cpu *cpu);

uint8_t inc_r8(struct cpu *cpu);
uint8_t inc_hl_ind(struct cpu *cpu);

uint8_t dec_r8(struct cpu *cpu);
uint8_t dec_hl_ind(struct cpu *cpu);

/// 16-bit arithmetic instructions
uint8_t add_hl_r16(struct cpu *cpu);
uint8_t inc_r16(struct cpu *cpu);
uint8_t dec_r16(struct cpu *cpu);

/// Bitwise logic instructions
uint8_t and_r8(struct cpu *cpu);
uint8_t and_hl_ind(struct cpu *cpu);
uint8_t and_n8(struct cpu *cpu);

uint8_t or_r8(struct cpu *cpu);
uint8_t or_hl_ind(struct cpu *cpu);
uint8_t or_n8(struct cpu *cpu);

uint8_t xor_r8(struct cpu *cpu);
uint8_t xor_hl_ind(struct cpu *cpu);
uint8_t xor_n8(struct cpu *cpu);

uint8_t cpl(struct cpu *cpu);

/// Bit flag instructions
uint8_t bit_b3_r8(struct cpu *cpu);
uint8_t bit_b3_hl_ind(struct cpu *cpu);

uint8_t res_b3_r8(struct cpu *cpu);
uint8_t res_b3_hl_ind(struct cpu *cpu);

uint8_t set_b3_r8(struct cpu *cpu);
uint8_t set_b3_hl_ind(struct cpu *cpu);

/// Bit shfit instructions
uint8_t rl_r8(struct cpu *cpu);
uint8_t rl_hl_ind(struct cpu *cpu);
uint8_t rla(struct cpu *cpu);

uint8_t rlc_r8(struct cpu *cpu);
uint8_t rlc_hl_ind(struct cpu *cpu);
uint8_t rlca(struct cpu *cpu);

uint8_t rr_r8(struct cpu *cpu);
uint8_t rr_hl_ind(struct cpu *cpu);
uint8_t rra(struct cpu *cpu);

uint8_t rrc_r8(struct cpu *cpu);
uint8_t rrc_hl_ind(struct cpu *cpu);
uint8_t rrca(struct cpu *cpu);

uint8_t sla_r8(struct cpu *cpu);
uint8_t sla_hl_ind(struct cpu *cpu);

uint8_t sra_r8(struct cpu *cpu);
uint8_t sra_hl_ind(struct cpu *cpu);

uint8_t srl_r8(struct cpu *cpu);
uint8_t srl_hl_ind(struct cpu *cpu);

uint8_t swap_r8(struct cpu *cpu);
uint8_t swap_hl_ind(struct cpu *cpu);

/// Jumps and subroutine instructions
uint8_t call_n16(struct cpu *cpu);
uint8_t call_cc_n16(struct cpu *cpu);

uint8_t jp_hl(struct cpu *cpu);
uint8_t jp_n16(struct cpu *cpu);
uint8_t jp_cc_n16(struct cpu *cpu);

uint8_t jr_e8(struct cpu *cpu);
uint8_t jr_cc_e8(struct cpu *cpu);

uint8_t ret_cc(struct cpu *cpu);
uint8_t ret(struct cpu *cpu);
uint8_t reti(struct cpu *cpu);

uint8_t rst_vec(struct cpu *cpu);

/// Carry flag instructions
uint8_t ccf(struct cpu *cpu);
uint8_t scf(struct cpu *cpu);

/// Stack manipulation instructions
uint8_t add_sp_e8(struct cpu *cpu);
uint8_t ld_hl_sp_e8(struct cpu *cpu);
uint8_t ld_n16_ind_sp(struct cpu *cpu);
uint8_t ld_sp_hl(struct cpu *cpu);
uint8_t pop_r16stk(struct cpu *cpu);
uint8_t push_r16stk(struct cpu *cpu);

/// Interrupt-related instructions
uint8_t di(struct cpu *cpu);
uint8_t ei(struct cpu *cpu);
uint8_t halt(struct cpu *cpu);

/// Misc.
uint8_t daa(struct cpu *cpu);
uint8_t nop(struct cpu *cpu);
uint8_t stop(struct cpu *cpu);
uint8_t prefix(struct cpu *cpu);
uint8_t illegal(struct cpu *cpu);
