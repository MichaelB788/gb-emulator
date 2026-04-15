#pragma once
#include "core/cpu.h"
#include <stdint.h>

// Load Instructions
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

// 8-bit arithmetic instructions
void ADC(CPU *cpu, uint8_t operand);

void adc_a_r8(CPU *cpu);

void adc_a_mem_hl(CPU *cpu);

void adc_a_n8(CPU *cpu);

void ADD(CPU *cpu, uint8_t operand);

void add_a_r8(CPU *cpu);

void add_a_mem_hl(CPU *cpu);

void add_a_n8(CPU *cpu);

void CP(CPU *cpu, uint8_t operand);

void cp_a_r8(CPU *cpu);

void cp_a_mem_hl(CPU *cpu);

void cp_a_n8(CPU *cpu);

uint8_t DEC(CPU *cpu, uint8_t operand);

void dec_r8(CPU *cpu);

void dec_mem_hl(CPU *cpu);

uint8_t INC(CPU *cpu, uint8_t operand);

void inc_r8(CPU *cpu);

void inc_mem_hl(CPU *cpu);

void SBC(CPU *cpu, uint8_t operand);

void sbc_a_r8(CPU *cpu);

void sbc_a_mem_hl(CPU *cpu);

void sbc_a_n8(CPU *cpu);

void SUB(CPU *cpu, uint8_t operand);

void sub_a_r8(CPU *cpu);

void sub_a_mem_hl(CPU *cpu);

void sub_a_n8(CPU *cpu);

// 16-bit arithmetic instructions
void add_hl_r16(CPU *cpu);

static inline void dec_r16(CPU *cpu) { --cpu->r16[op_y(cpu->opcode) >> 1]; }

static inline void inc_r16(CPU *cpu) { ++cpu->r16[op_y(cpu->opcode) >> 1]; }

// Bitwise logic instructions
void AND(CPU *cpu, uint8_t operand);

void and_a_r8(CPU *cpu);

void and_a_mem_hl(CPU *cpu);

void and_a_n8(CPU *cpu);

void cpl(CPU *cpu);

void OR(CPU *cpu, uint8_t operand);

void or_a_r8(CPU *cpu);

void or_a_mem_hl(CPU *cpu);

void or_a_n8(CPU *cpu);

void XOR(CPU *cpu, uint8_t operand);

void xor_a_r8(CPU *cpu);

void xor_a_mem_hl(CPU *cpu);

void xor_a_n8(CPU *cpu);

// Bit flag instructions
void BIT(CPU *cpu, uint8_t operand);

void bit_r8(CPU *cpu);

void bit_mem_hl(CPU *cpu);

void res_r8(CPU *cpu);

void res_mem_hl(CPU *cpu);

void set_r8(CPU *cpu);

void set_mem_hl(CPU *cpu);

// Bit shift instructions
uint8_t RL(CPU *cpu, uint8_t operand);

void rl_r8(CPU *cpu);

void rl_mem_hl(CPU *cpu);

void rla(CPU *cpu);

uint8_t RLC(CPU *cpu, uint8_t operand);

void rlc_r8(CPU *cpu);

void rlc_mem_hl(CPU *cpu);

void rlca(CPU *cpu);

uint8_t RR(CPU *cpu, uint8_t operand);

void rr_r8(CPU *cpu);

void rr_mem_hl(CPU *cpu);

void rra(CPU *cpu);

uint8_t RRC(CPU *cpu, uint8_t operand);

void rrc_r8(CPU *cpu);

void rrc_mem_hl(CPU *cpu);

void rrca(CPU *cpu);

uint8_t SLA(CPU *cpu, uint8_t operand);

void sla_r8(CPU *cpu);

void sla_mem_hl(CPU *cpu);

uint8_t SRA(CPU *cpu, uint8_t operand);

void sra_r8(CPU *cpu);

void sra_mem_hl(CPU *cpu);

uint8_t SRL(CPU *cpu, uint8_t operand);

void srl_r8(CPU *cpu);

void srl_mem_hl(CPU *cpu);

uint8_t SWAP(CPU *cpu, uint8_t operand);

void swap_r8(CPU *cpu);

void swap_mem_hl(CPU *cpu);

// Jumps and subroutine instructions
void call_a16(CPU *cpu);

void call_a16_cc(CPU *cpu);

static inline void jp_hl(CPU *cpu) { cpu->PC = cpu->HL.word; }

static inline void jp_a16(CPU *cpu) { cpu->PC = read_n16(cpu); }

void jp_cc_a16(CPU *cpu);

static inline void jr_e8(CPU *cpu) { cpu->PC += (int8_t)read_n8(cpu); }

void jr_e8_cc(CPU *cpu);

void ret(CPU *cpu);

void ret_cc(CPU *cpu);

void reti(CPU *cpu);

void rst_vec(CPU *cpu);

// Interrupt-related instructions
void halt(CPU *cpu);

// Misc.
static inline void nop(CPU *cpu) {};

static inline void stop(CPU *cpu) {};

void illegal(CPU *cpu);
