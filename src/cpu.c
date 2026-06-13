#include "cpu.h"
#include "gameboy.h"
#include <stdint.h>

void init_cpu(CPU *cpu, GameBoy *gameboy) {
  cpu->gameboy = gameboy;
  cpu->r8[REG_B] = 0xFF;
  cpu->r8[REG_C] = 0x13;
  cpu->r8[REG_D] = 0x00;
  cpu->r8[REG_E] = 0xC1;
  cpu->r8[REG_H] = 0x84;
  cpu->r8[REG_L] = 0x03;
  cpu->r8[REG_A] = 0x01;
  cpu->r8[REG_F] = 0x00;
  cpu->PC = 0x0100;
  cpu->SP = 0xFFFE;
}

void set_r8_pair(CPU *cpu, R16_Idx r16_idx, uint16_t val) {
  cpu->r8[r16_idx] = val >> 8;
  cpu->r8[r16_idx + 1] = val & 0xFF;
}

uint16_t get_r8_pair(const CPU *cpu, R16_Idx r16_idx) {
  return (uint16_t)cpu->r8[r16_idx] << 8 | cpu->r8[r16_idx + 1];
}

uint8_t fetch_byte(CPU *cpu) { return read_byte(cpu->gameboy, cpu->PC++); }

void set_z(CPU *cpu, bool val) {
  if (val) {
    cpu->r8[REG_F] |= 0x80;
  } else {
    cpu->r8[REG_F] &= ~0x80;
  }
}

void set_n(CPU *cpu, bool val) {
  if (val) {
    cpu->r8[REG_F] |= 0x40;
  } else {
    cpu->r8[REG_F] &= ~0x40;
  }
}

void set_h(CPU *cpu, bool val) {
  if (val) {
    cpu->r8[REG_F] |= 0x20;
  } else {
    cpu->r8[REG_F] &= ~0x20;
  }
}

void set_c(CPU *cpu, bool val) {
  if (val) {
    cpu->r8[REG_F] |= 0x10;
  } else {
    cpu->r8[REG_F] &= ~0x10;
  }
}

bool is_z_set(CPU *cpu) { return cpu->r8[REG_F] & 0x80; }

bool is_n_set(CPU *cpu) { return cpu->r8[REG_F] & 0x40; }

bool is_h_set(CPU *cpu) { return cpu->r8[REG_F] & 0x20; }

bool is_c_set(CPU *cpu) { return cpu->r8[REG_F] & 0x10; }
