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

// Interrupt-related instructions
void halt(CPU *cpu);

// Misc.
static inline void nop(CPU *cpu) {};

static inline void stop(CPU *cpu) {};
