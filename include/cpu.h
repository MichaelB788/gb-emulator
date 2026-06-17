#pragma once
#include <stdbool.h>
#include <stdint.h>

// ISA indicies for the regular 8-bit registers.
typedef enum {
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
} R8_Idx;

// Offsets to form the regular 8-bit register pairs, such that
// hi = r8[i], and lo = r8[i + 1]
typedef enum { REG_BC = 0, REG_DE = 2, REG_HL = 4 } R16_Idx;

typedef struct CPU {
  uint8_t r8[8]; // B, C, D, E, H, L, F, A
  bool IME;
  uint16_t PC;
  uint16_t SP;
} CPU;

void init_cpu(CPU *cpu);

void set_r8_pair(CPU *cpu, R16_Idx r16_idx, uint16_t val);
uint16_t get_r8_pair(const CPU *cpu, R16_Idx r16_idx);

void set_z(CPU *cpu, bool val);
void set_n(CPU *cpu, bool val);
void set_h(CPU *cpu, bool val);
void set_c(CPU *cpu, bool val);

bool is_z_set(CPU *cpu);
bool is_n_set(CPU *cpu);
bool is_h_set(CPU *cpu);
bool is_c_set(CPU *cpu);
