#include "cpu.h"
#include <stdint.h>

void init_cpu(struct cpu *cpu) {
  cpu->B = 0xFF;
  cpu->C = 0x13;
  cpu->D = 0x00;
  cpu->E = 0xC1;
  cpu->H = 0x84;
  cpu->L = 0x03;
  cpu->A = 0x01;
  cpu->F = 0x00;
  cpu->PC = 0x0100;
  cpu->SP = 0xFFFE;
}

void set_hl(struct cpu *cpu, uint16_t val) {
  cpu->H = val >> 8;
  cpu->L = val & 0xFF;
}

uint16_t get_hl(struct cpu *cpu) { return (uint16_t)cpu->H << 8 | cpu->L; }

void set_regpair(struct cpu *cpu, enum regpair rp, uint16_t val) {
  cpu->r8[rp] = val >> 8;
  cpu->r8[rp + 1] = val & 0xFF;
}

uint16_t get_regpair(const struct cpu *cpu, enum regpair rp) {
  return (uint16_t)cpu->r8[rp] << 8 | cpu->r8[rp + 1];
}

void set_flag(uint8_t *reg_f, enum flag flag, bool val) {
  if (val) {
    *reg_f |= flag;
  } else {
    *reg_f &= ~flag;
  }
}

bool is_flag_set(uint8_t reg_f, enum flag flag) { return reg_f & flag; }

bool get_carry(uint8_t reg_f) { return reg_f & FLAG_C; }
