#pragma once
#include "cpu.h"
#include "gameboy.h"
#include <stdint.h>

/**
 * LOAD INSTRUCTIONS
 */

static uint8_t field_y(uint8_t opcode) { return (opcode >> 3) & 0b111; }

static uint8_t field_z(uint8_t opcode) { return opcode & 0b111; }

static uint8_t fetch_n8(struct gameboy *gb) {
  return read_byte(gb, gb->cpu.PC++);
}

static uint8_t get_hl_ind(struct gameboy *gb) {
  return read_byte(gb, get_hl(&gb->cpu));
}

static void set_hl_ind(struct gameboy *gb, uint8_t val) {
  write_byte(gb, get_hl(&gb->cpu), val);
}

static uint16_t fetch_n16(struct gameboy *gb) {
  const uint8_t lo = read_byte(gb, gb->cpu.PC++);
  const uint8_t hi = read_byte(gb, gb->cpu.PC++);
  return (uint16_t)hi << 8 | lo;
}

int ld_r8_r8(struct gameboy *gb) {
  const uint8_t op = gb->instruction_register;
  const uint8_t dest = field_y(op);
  const uint8_t src = field_z(op);
  gb->cpu.r8[dest] = gb->cpu.r8[src];
  return 4;
}

int ld_r8_n8(struct gameboy *gb) {
  const uint8_t op = gb->instruction_register;
  const uint8_t dest = field_y(op);
  gb->cpu.r8[dest] = fetch_n8(gb);
  return 8;
}

int ld_r16_n16(struct gameboy *gb) {
  const uint8_t op = gb->instruction_register;
  const regpair_t dest = field_y(op) & 0b110;
  set_regpair(&gb->cpu, dest, fetch_n16(gb));
  return 12;
}

int ld_hl_ind_r8(struct gameboy *gb) {
  const uint8_t op = gb->instruction_register;
  const uint8_t src = field_z(op);
  set_hl_ind(gb, gb->cpu.r8[src]);
  return 8;
}

int ld_hl_ind_n8(struct gameboy *gb) {
  set_hl_ind(gb, fetch_n8(gb));
  return 8;
}

int ld_r8_hl_ind(struct gameboy *gb) {
  const uint8_t op = gb->instruction_register;
  const uint8_t dest = field_z(op);
  gb->cpu.r8[dest] = get_hl_ind(gb);
  return 8;
}

int ld_r16_ind_a(struct gameboy *gb) {
  const uint8_t op = gb->instruction_register;
  const regpair_t src = field_y(op) & 0b110;
  write_byte(gb, get_regpair(&gb->cpu, src), gb->cpu.r8[REG_A]);
  return 8;
}

int ld_n16_ind_a(struct gameboy *gb) {
  write_byte(gb, fetch_n16(gb), gb->cpu.r8[REG_A]);
  return 16;
}

int ldh_n8_ind_a(struct gameboy *gb) {
  write_byte(gb, 0xFF00 | fetch_n8(gb), gb->cpu.r8[REG_A]);
  return 12;
}

int ldh_c_ind_a(struct gameboy *gb) {
  write_byte(gb, 0xFF00 | gb->cpu.r8[REG_C], gb->cpu.r8[REG_A]);
  return 8;
}

int ld_a_c_ind(struct gameboy *gb) {
  gb->cpu.r8[REG_A] = read_byte(gb, 0xFF00 | gb->cpu.r8[REG_C]);
  return 8;
}
