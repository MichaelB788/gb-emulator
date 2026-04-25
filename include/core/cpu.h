#pragma once
#include "core/bus.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
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

// Links the CPU to the bus and intializes pointer to member arrays.
void init_cpu(CPU *cpu, Bus *bus);

uint8_t step(CPU *cpu);

void log_ins(CPU *cpu, Instruction *ins);

uint8_t op_y(uint8_t op);

uint8_t op_z(uint8_t op);

uint8_t *r8(CPU *cpu);

uint8_t *r8_dest(CPU *cpu);

uint16_t *r16(CPU *cpu);

bool check_cc(CPU *cpu);

uint8_t read_n8(CPU *cpu);

uint8_t read_hl(CPU *cpu);

uint16_t read_n16(CPU *cpu);

void write_hl(CPU *cpu, uint8_t val);

void set_flag(CPU *cpu, Flag flag, bool val);

bool get_flag(CPU *cpu, Flag flag);
