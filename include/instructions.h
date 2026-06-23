#pragma once
#include "cpu.h"
#include "gameboy.h"
#include <stdint.h>

uint8_t field_y(uint8_t opcode);

uint8_t field_z(uint8_t opcode);

uint8_t fetch_n8(struct gameboy *gb);

uint8_t read_hl(struct gameboy *gb);

void write_hl(struct gameboy *gb, uint8_t val);

uint16_t fetch_n16(struct gameboy *gb);

enum condition { COND_NZ = 0, COND_Z = 1, COND_NC = 2, COND_C = 3 };
bool check_condition(struct cpu *cpu, enum condition cond);

void push_onto_stack(struct gameboy *gb, uint16_t val);

uint16_t pop_off_stack(struct gameboy *gb);

/**
 * Load instructions
 */

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

int ldh_a_c_ind(struct gameboy *gb);

int ld_hli_ind_a(struct gameboy *gb);

int ld_hld_ind_a(struct gameboy *gb);

int ld_a_hli_ind(struct gameboy *gb);

int ld_a_hld_ind(struct gameboy *gb);

/**
 * 8-bit arithmetic instructions
 */

void add_impl(struct cpu *cpu, uint8_t operand, bool carry);
void sub_impl(struct cpu *cpu, uint8_t operand, bool carry);
void cp_impl(struct cpu *cpu, uint8_t operand);
uint8_t inc_n8_impl(struct cpu *cpu, uint8_t operand);
uint8_t dec_n8_impl(struct cpu *cpu, uint8_t operand);

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

/**
 * Misc.
 */

void daa(struct cpu *cpu);

void illegal(struct gameboy *gb);
