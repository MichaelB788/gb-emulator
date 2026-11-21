#pragma once
#include <stdint.h>

struct dmgb;

void dmgb_add_u8(struct dmgb *dmgb, uint8_t u8);
void dmgb_adc_u8(struct dmgb *dmgb, uint8_t u8);
void dmgb_sub_u8(struct dmgb *dmgb, uint8_t u8);
void dmgb_sbc_u8(struct dmgb *dmgb, uint8_t u8);
void dmgb_cp_u8(struct dmgb *dmgb, uint8_t u8);

uint8_t dmgb_inc_u8(struct dmgb *dmgb, uint8_t u8);
uint8_t dmgb_dec_u8(struct dmgb *dmgb, uint8_t u8);

void dmgb_add_r16(struct dmgb *dmgb, uint16_t u16); // M-cycles: 1

// M-cycles: 1 if setting HL, 2 if setting SP
uint16_t dmgb_add_sp_i8(struct dmgb *dmgb, int8_t i8);

void dmgb_and_u8(struct dmgb *dmgb, uint8_t u8);
void dmgb_xor_u8(struct dmgb *dmgb, uint8_t u8);
void dmgb_or_u8(struct dmgb *dmgb, uint8_t u8);

void dmgb_bit_b3_u8(struct dmgb *dmgb, uint8_t b3, uint8_t u8);

uint8_t dmgb_rl_u8(struct dmgb *dmgb, uint8_t u8);
uint8_t dmgb_rlc_u8(struct dmgb *dmgb, uint8_t u8);

uint8_t dmgb_rr_u8(struct dmgb *dmgb, uint8_t u8);
uint8_t dmgb_rrc_u8(struct dmgb *dmgb, uint8_t u8);

uint8_t dmgb_sla_u8(struct dmgb *dmgb, uint8_t u8);
uint8_t dmgb_sra_u8(struct dmgb *dmgb, uint8_t u8);
uint8_t dmgb_srl_u8(struct dmgb *dmgb, uint8_t u8);

uint8_t dmgb_swap_u8(struct dmgb *dmgb, uint8_t u8);

void dmgb_daa(struct dmgb *dmgb);
void dmgb_halt(struct dmgb *dmgb);
void dmgb_illegal(struct dmgb *dmgb, uint8_t opcode);
