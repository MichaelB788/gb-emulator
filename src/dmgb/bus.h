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

// M-cycles: 1; increments PC
uint8_t dmgb_fetch_u8(struct dmgb *dmgb);

// M-cycles: 1; increments PC
int8_t dmgb_fetch_s8(struct dmgb *dmgb);

// M-cycles: 2; increments PC by 2
uint16_t dmgb_fetch_u16(struct dmgb *dmgb);

// M-cycles: 1
void dmgb_write_u8(struct dmgb *dmgb, uint16_t a16, uint8_t u8);

// M-cycles: 2
void dmgb_write_u16(struct dmgb *dmgb, uint16_t a16, uint16_t u16);

// M-cycles: 2; decrements SP by 2 (stack grows downward)
void dmgb_push_u16(struct dmgb *dmgb, uint16_t u16);

// M-cycles: 2; increments SP by 2 (stack shrinks upward)
uint16_t dmgb_pop_u16(struct dmgb *dmgb);

// M-cycles: 0 untaken / 1 taken; sets PC to to a16
void dmgb_jump(struct dmgb *dmgb, uint16_t a16, bool cond);

// M-cycles: 0 untaken / 1 taken; adds s8 to PC
void dmgb_jump_rotation(struct dmgb *dmgb, int8_t s8, bool cond);

// M-cycles: 0 untaken / 3 taken; pushes PC onto the stack and set PC to a16
void dmgb_call(struct dmgb *dmgb, uint16_t a16, bool cond);

// M-cycles: 0 untaken / 3 taken; pops an a16 off of the stack into PC
void dmgb_return(struct dmgb *dmgb, bool cond);
