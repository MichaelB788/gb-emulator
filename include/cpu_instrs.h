#pragma once
#include <stdint.h>

typedef struct CPU CPU;

//
// Load Instructions
//

void ld_r8_r8(CPU *cpu);

void ld_r8_n8(CPU *cpu);

void ld_r16_n16(CPU *cpu);

void ld_mem_hl_r8(CPU *cpu);

void ld_mem_hl_n8(CPU *cpu);

void ld_r8_mem_hl(CPU *cpu);

void ld_mem_r16_a(CPU *cpu);

void ld_mem_n16_a(CPU *cpu);

void ldh_mem_n8_a(CPU *cpu);

void ldh_mem_c_a(CPU *cpu);

void ld_a_mem_r16(CPU *cpu);

void ld_a_mem_n16(CPU *cpu);

void ldh_a_mem_n8(CPU *cpu);

void ldh_a_mem_c(CPU *cpu);

void ld_mem_hli_a(CPU *cpu);

void ld_mem_hld_a(CPU *cpu);

void ld_a_mem_hli(CPU *cpu);

void ld_a_mem_hld(CPU *cpu);

//
// 8-bit arithmetic instructions
//

void adc_a_r8(CPU *cpu);

void adc_a_mem_hl(CPU *cpu);

void adc_a_n8(CPU *cpu);

void add_a_r8(CPU *cpu);

void add_a_mem_hl(CPU *cpu);

void add_a_n8(CPU *cpu);

void cp_a_r8(CPU *cpu);

void cp_a_mem_hl(CPU *cpu);

void cp_a_n8(CPU *cpu);

void dec_r8(CPU *cpu);

void dec_mem_hl(CPU *cpu);

void inc_r8(CPU *cpu);

void inc_mem_hl(CPU *cpu);

void sbc_a_r8(CPU *cpu);

void sbc_a_mem_hl(CPU *cpu);

void sbc_a_n8(CPU *cpu);

void sub_a_r8(CPU *cpu);

void sub_a_mem_hl(CPU *cpu);

void sub_a_n8(CPU *cpu);

//
// 16-bit arithmetic instructions
//

void add_hl_r16(CPU *cpu);

void dec_r16(CPU *cpu);

void inc_r16(CPU *cpu);

//
// Bitwise logic instructions
//

void and_a_r8(CPU *cpu);

void and_a_mem_hl(CPU *cpu);

void and_a_n8(CPU *cpu);

void cpl(CPU *cpu);

void or_a_r8(CPU *cpu);

void or_a_mem_hl(CPU *cpu);

void or_a_n8(CPU *cpu);

void xor_a_r8(CPU *cpu);

void xor_a_mem_hl(CPU *cpu);

void xor_a_n8(CPU *cpu);

//
// Bit flag instructions
//

void bit_r8(CPU *cpu);

void bit_mem_hl(CPU *cpu);

void res_r8(CPU *cpu);

void res_mem_hl(CPU *cpu);

void set_r8(CPU *cpu);

void set_mem_hl(CPU *cpu);

//
// Bit shift instructions
//

void rl_r8(CPU *cpu);

void rl_mem_hl(CPU *cpu);

void rla(CPU *cpu);

void rlc_r8(CPU *cpu);

void rlc_mem_hl(CPU *cpu);

void rlca(CPU *cpu);

void rr_r8(CPU *cpu);

void rr_mem_hl(CPU *cpu);

void rra(CPU *cpu);

void rrc_r8(CPU *cpu);

void rrc_mem_hl(CPU *cpu);

void rrca(CPU *cpu);

void sla_r8(CPU *cpu);

void sla_mem_hl(CPU *cpu);

void sra_r8(CPU *cpu);

void sra_mem_hl(CPU *cpu);

void srl_r8(CPU *cpu);

void srl_mem_hl(CPU *cpu);

void swap_r8(CPU *cpu);

void swap_mem_hl(CPU *cpu);

//
// Jumps and subroutine instructions
//

void call_a16(CPU *cpu);

void call_cc_a16(CPU *cpu);

void jp_hl(CPU *cpu);

void jp_a16(CPU *cpu);

void jp_cc_a16(CPU *cpu);

void jr_e8(CPU *cpu);

void jr_cc_e8(CPU *cpu);

void ret(CPU *cpu);

void ret_cc(CPU *cpu);

void reti(CPU *cpu);

void rst_vec(CPU *cpu);

//
// Carry flag instructions
//

void ccf(CPU *cpu);

void scf(CPU *cpu);

//
// Stack manipulation instructions
//

void add_sp_e8(CPU *cpu);

void ld_mem_n16_sp(CPU *cpu);

void ld_hl_sp_e8(CPU *cpu);

void ld_sp_hl(CPU *cpu);

void pop_r16(CPU *cpu);

void pop_af(CPU *cpu);

void push_r16(CPU *cpu);

void push_af(CPU *cpu);

//
// Interrupt-related instructions
//

void di(CPU *cpu);

void ei(CPU *cpu);

void halt(CPU *cpu);

//
// Misc.
//

void nop(CPU *cpu);

void stop_n8(CPU *cpu);

void daa(CPU *cpu);

void prefix(CPU *cpu);

void illegal(CPU *cpu);
