#pragma once
#include <stdbool.h>
#include <stdint.h>

// ISA indicies for the regular 8-bit registers.
enum {
  REG_B = 0,
  REG_C = 1,
  REG_D = 2,
  REG_E = 3,
  REG_H = 4,
  REG_L = 5,
  REG_A = 7,
  REG_F = 6 // Typically this index would map to [HL], but since [HL] is a
            // memory access rather than an actual register, this index points
            // to the flags register instead.
};

// Offsets to form the regular 8-bit register pairs, such that
// hi = r8[i], and lo = r8[i + 1]
enum r16_idx { REG_BC = 0, REG_DE = 2, REG_HL = 4 };

struct cpu {
  uint8_t r8[8]; // B, C, D, E, H, L, F, A
  bool IME;
  uint16_t PC;
  uint16_t SP;
};

void init_cpu(struct cpu *cpu);

void set_r8_pair(struct cpu *cpu, enum r16_idx r16_idx, uint16_t val);
uint16_t get_r8_pair(const struct cpu *cpu, enum r16_idx r16_idx);

enum flag { FLAG_Z = 0x80, FLAG_N = 0x40, FLAG_H = 0x20, FLAG_C = 0x10 };
void set_flag(struct cpu *cpu, enum flag flag, bool val);
bool is_flag_set(const struct cpu *cpu, enum flag flag);
