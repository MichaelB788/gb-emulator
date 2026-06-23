#pragma once
#include "gameboy.h"
#include <stdint.h>

/**
 * LOAD INSTRUCTIONS
 */

uint8_t field_y(uint8_t opcode) { return (opcode >> 3) & 0b111; }

uint8_t field_z(uint8_t opcode) { return opcode & 0b111; }

int ld_r8_r8(struct gameboy *gb) {
  const uint8_t op = gb->instruction_register;
  uint8_t dest = field_y(op);
  uint8_t src = field_z(op);
  gb->cpu.r8[dest] = gb->cpu.r8[src];
  return 4;
}
