#pragma once
#include <stdbool.h>
#include <stdint.h>

/**
 * The GameBoy's CPU
 */
struct cpu {
  union {
    struct {
      uint8_t B, C, D, E, H, L, F, A;
    };
    uint8_t r8[8];
  };
  bool IME;
  uint16_t PC;
  uint16_t SP;
};

void init_cpu(struct cpu *cpu);

void set_hl(struct cpu *cpu, uint16_t val);

uint16_t get_hl(struct cpu *cpu);

/**
 * Offsets to form the regular 8-bit register pairs, such that
 * hi = r8[i], and lo = r8[i + 1]
 *
 * Note: 16-bit registers are derived from the y-field, yet the offset can be
 * deduced simply by ignoring the first bit, the binary representations
 * exemplify this.
 */
enum regpair { REG_BC = 0b000, REG_DE = 0b010, REG_HL = 0b100 };

void set_regpair(struct cpu *cpu, enum regpair rp, uint16_t val);

uint16_t get_regpair(const struct cpu *cpu, enum regpair rp);

// Flags used by the CPU. The values mask the bit corresponding to the flag.
enum flag { FLAG_Z = 0x80, FLAG_N = 0x40, FLAG_H = 0x20, FLAG_C = 0x10 };

void set_flag(uint8_t *reg_f, enum flag flag, bool val);

bool is_flag_set(uint8_t reg_f, enum flag flag);

bool get_carry(uint8_t reg_f);
