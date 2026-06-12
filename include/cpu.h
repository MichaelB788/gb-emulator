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

typedef struct GameBoy GameBoy;
typedef struct CPU {
  uint8_t r8[8];   // B, C, D, E, H, L, F, A
  uint8_t field_y; // Bit field y of the ISA pattern [xx yyy zzz]
  uint8_t field_z; // Bit field z of the ISA pattern [xx yyy zzz]
  bool IME;
  uint16_t PC;
  uint16_t SP;
  GameBoy *gameboy;
} CPU;

void init_cpu(CPU *cpu, GameBoy *gameboy);

void set_r16(CPU *cpu, R16_Idx r16_idx, uint16_t val);
uint16_t get_r16(const CPU *cpu, R16_Idx r16_idx);

uint8_t fetch_imm8(CPU *cpu);
uint16_t fetch_imm16(CPU *cpu);
uint8_t read_hl(CPU *cpu);
void write_hl(CPU *cpu);

void set_z(CPU *cpu, bool val);
void set_n(CPU *cpu, bool val);
void set_h(CPU *cpu, bool val);
void set_c(CPU *cpu, bool val);

bool is_z_set(CPU *cpu);
bool is_n_set(CPU *cpu);
bool is_h_set(CPU *cpu);
bool is_c_set(CPU *cpu);

typedef enum { COND_NZ = 0, COND_Z = 1, COND_NC = 2, COND_C = 3 } Condition;
bool check_condition(CPU *cpu, Condition cond);
