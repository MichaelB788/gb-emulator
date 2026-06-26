#include "cpu.h"
#include <stdint.h>

void init_cpu(struct cpu *cpu) {
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

void set_regpair(struct cpu *cpu, enum regpair rp, uint16_t val) {
  cpu->r8[rp] = val >> 8;
  cpu->r8[rp + 1] = val & 0xFF;
}

uint16_t get_regpair(const struct cpu *cpu, enum regpair rp) {
  return (uint16_t)cpu->r8[rp] << 8 | cpu->r8[rp + 1];
}

void set_hl(struct cpu *cpu, uint16_t val) {
  cpu->r8[REG_H] = val >> 8;
  cpu->r8[REG_L] = val & 0xFF;
}

uint16_t get_hl(struct cpu *cpu) {
  return (uint16_t)cpu->r8[REG_H] << 8 | cpu->r8[REG_L];
}

void set_flag(struct cpu *cpu, enum flag flag, bool val) {
  if (val) {
    cpu->r8[REG_F] |= flag;
  } else {
    cpu->r8[REG_F] &= ~flag;
  }
}
bool is_flag_set(const struct cpu *cpu, enum flag flag) {
  return cpu->r8[REG_F] & flag;
}

bool get_carry(const struct cpu *cpu) { return cpu->r8[REG_F] & FLAG_C; }
