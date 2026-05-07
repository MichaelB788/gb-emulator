#pragma once
#include "core/bus.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Values set for each flag correspond to their bit index. For more info, see:
 * https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-flags-register-lower-8-bits-of-af-register
 */
typedef enum { FLAG_Z = 7, FLAG_N = 6, FLAG_H = 5, FLAG_C = 4 } Flag;

typedef enum { CPU_RUNNING, CPU_STOPPED, CPU_HALTED } CPU_State;

typedef struct {
  union {
    struct {
      uint8_t C, B;
    };
    uint16_t BC;
  };
  union {
    struct {
      uint8_t E, D;
    };
    uint16_t DE;
  };
  union {
    struct {
      uint8_t L, H;
    };
    uint16_t HL;
  };
  union {
    struct {
      uint8_t F, A;
    };
    uint16_t AF;
  };

  uint16_t PC, SP;
  uint8_t opcode, cycles_taken;
  bool IME;
  CPU_State state;

  uint8_t *r8[8];   // B, C, D, E, H, L, F, A
  uint16_t *r16[4]; // BC, DE, HL, SP
  Bus *bus;
} CPU;

typedef struct {
  const char *name;
  uint8_t cycles;
  void (*exec)(CPU *);
} Instruction;

void init_cpu(CPU *cpu, Bus *bus);

uint8_t step(CPU *cpu);

void log_ins(const CPU *cpu, const Instruction *ins);

// Obtains the y field of the opcode bit pattern [xx yyy zzz].
uint8_t op_y(uint8_t op);

// Obtains the z field of the opcode bit pattern [xx yyy zzz].
uint8_t op_z(uint8_t op);

/**
 * Returns a pointer to an 8-bit CPU register based on the current opcode.
 *
 * Note: Attempting to access what should be [HL] is undefined behavior.
 */
uint8_t *r8(CPU *cpu);

/**
 * Returns a pointer to an 8-bit CPU register based on the current opcode.
 * This function should only be used in register-to-register loads.
 *
 * Note: Attempting to access what should be [HL] is undefined behavior.
 */
uint8_t *r8_dest(CPU *cpu);

/**
 * Obtains a pointer to a 16-bit CPU register based on the current opcode.
 * This includes: [BC, DE, HL, SP]
 */
uint16_t *r16(CPU *cpu);

bool check_cc(const CPU *cpu);

uint8_t fetch_byte(CPU *cpu);

uint8_t read_hl(const CPU *cpu);

uint16_t fetch_word(CPU *cpu);

void write_hl(CPU *cpu, uint8_t val);

void set_flag(CPU *cpu, Flag flag, bool val);

bool get_flag(const CPU *cpu, Flag flag);
