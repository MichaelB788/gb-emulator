#pragma once
#include <stdbool.h>
#include <stdint.h>

struct dmgb;

uint8_t dmgb_bus_read(const struct dmgb *dmgb, uint16_t a16);
void dmgb_bus_write(struct dmgb *dmgb, uint16_t a16, uint8_t u8);

// M-cycles: 1
uint8_t dmgb_read_u8(struct dmgb *dmgb, uint16_t a16);

// M-cycles: 2
uint16_t dmgb_read_u16(struct dmgb *dmgb, uint16_t a16);

// M-cycles: 1
uint8_t dmgb_read_imm8(struct dmgb *dmgb);

// M-cycles: 1
uint8_t dmgb_read_hl_u8(struct dmgb *dmgb);

// M-cycles: 2
uint16_t dmgb_read_imm16(struct dmgb *dmgb);

// M-cycles: 1
void dmgb_write_u8(struct dmgb *dmgb, uint16_t a16, uint8_t u8);

// M-cycles: 1
void dmgb_write_hl_u8(struct dmgb *dmgb, uint8_t u8);

// M-cycles: 2
void dmgb_write_u16(struct dmgb *dmgb, uint16_t a16, uint16_t u16);

// M-cycles: 2
void dmgb_push_u16(struct dmgb *dmgb, uint16_t u16);

// M-cycles: 2
uint16_t dmgb_pop_u16(struct dmgb *dmgb);

// M-cycles: 0 untaken / 1 taken
void dmgb_jump(struct dmgb *dmgb, uint16_t a16, bool cond);

// M-cycles: 0 untaken / 1 taken
void dmgb_jump_rotation(struct dmgb *dmgb, int8_t i8, bool cond);

// M-cycles: 0 untaken / 3 taken
void dmgb_call(struct dmgb *dmgb, uint16_t a16, bool cond);

// M-cycles: 0 untaken / 3 taken
void dmgb_return(struct dmgb *dmgb, bool cond);
