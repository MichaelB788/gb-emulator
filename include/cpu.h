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
} Reg8_Idx;

// Offsets to form the regular 8-bit register pairs, such that
// hi = r8[i], and lo = r8[i + 1]
typedef enum { REG_BC = 0, REG_DE = 2, REG_HL = 4 } Reg16_Idx;

typedef struct {
  uint8_t r8[8];   // B, C, D, E, H, L, F, A
  uint8_t field_y; // Bit field y of the ISA pattern [xx yyy zzz]
  uint8_t field_z; // Bit field z of the ISA pattern [xx yyy zzz]
  bool IME;
  uint16_t PC;
  uint16_t SP;
} CPU;

typedef struct {
  const char *name;
  uint8_t cycles;
  void (*exec)(CPU *);
} Instruction;

void init_cpu(CPU *cpu);

void set_r16(CPU *cpu, Reg16_Idx r16_idx, uint16_t val);
uint16_t get_r16(const CPU *cpu, Reg16_Idx r16_idx);

void set_z(uint8_t *flags, bool val);
void set_n(uint8_t *flags, bool val);
void set_h(uint8_t *flags, bool val);
void set_c(uint8_t *flags, bool val);

bool is_z_set(uint8_t flags);
bool is_n_set(uint8_t flags);
bool is_h_set(uint8_t flags);
bool is_c_set(uint8_t flags);

typedef enum { COND_NZ = 0, COND_Z = 1, COND_NC = 2, COND_C = 3 } Condition;
bool check_condition(uint8_t flags, Condition cond);
