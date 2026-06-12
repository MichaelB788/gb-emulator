#include "cpu.h"
#include "gameboy.h"
#include <stdint.h>

void init_cpu(CPU *cpu, GameBoy *gameboy) {
  cpu->gameboy = gameboy;
  cpu->r8[0] = 0xFF;
  cpu->r8[1] = 0x13;
  cpu->r8[2] = 0x00;
  cpu->r8[3] = 0xC1;
  cpu->r8[4] = 0x84;
  cpu->r8[5] = 0x03;
  cpu->r8[6] = 0x00;
  cpu->r8[7] = 0x01;
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

uint16_t fetch_word(CPU *cpu) {
  const uint8_t lo = read_byte(cpu->gameboy, cpu->PC++);
  const uint8_t hi = read_byte(cpu->gameboy, cpu->PC++);
  return (uint16_t)hi << 8 | lo;
}

uint8_t fetch_byte(CPU *cpu) { return read_byte(cpu->gameboy, cpu->PC++); }

uint8_t read_hl(CPU *cpu) {
  return read_byte(cpu->gameboy, get_r8_pair(cpu, REG_HL));
}

void write_hl(CPU *cpu, uint8_t val) {
  write_byte(cpu->gameboy, get_r8_pair(cpu, REG_HL), val);
}

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

bool check_condition(CPU *cpu, Condition cond) {
  switch (cond) {
  case COND_NZ:
    return !is_z_set(cpu);
  case COND_Z: // Z
    return is_z_set(cpu);
  case COND_NC: // NC
    return !is_c_set(cpu);
  case COND_C: // C
    return is_c_set(cpu);
  }
}
