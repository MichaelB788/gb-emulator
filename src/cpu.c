#include "cpu.h"
#include "bitwise.h"
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

void set_r8_pair(struct cpu *cpu, enum r16_idx r16_idx, uint16_t val) {
  cpu->r8[r16_idx] = val >> 8;
  cpu->r8[r16_idx + 1] = val & 0xFF;
}

uint16_t get_r8_pair(const struct cpu *cpu, enum r16_idx r16_idx) {
  return (uint16_t)cpu->r8[r16_idx] << 8 | cpu->r8[r16_idx + 1];
}

void set_z(struct cpu *cpu, bool val) { set_bit(&cpu->r8[REG_F], 7, val); }
void set_n(struct cpu *cpu, bool val) { set_bit(&cpu->r8[REG_F], 6, val); }
void set_h(struct cpu *cpu, bool val) { set_bit(&cpu->r8[REG_F], 5, val); }
void set_c(struct cpu *cpu, bool val) { set_bit(&cpu->r8[REG_F], 4, val); }

bool is_z_set(struct cpu *cpu) { return get_bit(cpu->r8[REG_F], 7); }
bool is_n_set(struct cpu *cpu) { return get_bit(cpu->r8[REG_F], 6); }
bool is_h_set(struct cpu *cpu) { return get_bit(cpu->r8[REG_F], 5); }
bool is_c_set(struct cpu *cpu) { return get_bit(cpu->r8[REG_F], 4); }
