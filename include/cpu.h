#pragma once
#include <stdbool.h>
#include <stdint.h>

// ISA indicies for the regular 8-bit registers.
enum reg8 {
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

/**
 * Offsets to form the regular 8-bit register pairs, such that
 * hi = r8[i], and lo = r8[i + 1]
 *
 * Note: 16-bit registers are derived from the y-field, yet the offset can be
 * deduced simply by ignoring the first bit, the binary representations
 * exemplify this.
 */
enum regpair { REG_BC = 0b000, REG_DE = 0b010, REG_HL = 0b100 };

/**
 * The GameBoy's CPU
 */
struct cpu {
  uint8_t r8[8]; // B, C, D, E, H, L, F, A
  bool IME;
  uint16_t PC;
  uint16_t SP;
};

void init_cpu(struct cpu *cpu);

void set_regpair(struct cpu *cpu, enum regpair reg, uint16_t val);

uint16_t get_regpair(const struct cpu *cpu, enum regpair reg);

void set_hl(struct cpu *cpu, uint16_t val);

uint16_t get_hl(struct cpu *cpu);

// Flags used by the CPU. The values mask the bit corresponding to the flag.
enum flag { FLAG_Z = 0x80, FLAG_N = 0x40, FLAG_H = 0x20, FLAG_C = 0x10 };

void set_flag(struct cpu *cpu, enum flag flag, bool val);

bool is_flag_set(const struct cpu *cpu, enum flag flag);

bool get_carry(const struct cpu *cpu);
