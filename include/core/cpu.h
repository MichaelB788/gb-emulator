#pragma once
#include "core/bus.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum { FLAG_C = 4, FLAG_H, FLAG_N, FLAG_Z } Flag;

typedef union {
  struct {
    uint8_t lo, hi;
  } byte;
  uint16_t word;
} RegisterPair;

typedef struct {
  RegisterPair BC, DE, HL;

  uint8_t A, F, opcode;

  uint16_t PC, SP;

  // B, C, D, E, H, L, F, A
  uint8_t *r8[8];

  // BC, DE, HL, SP
  uint16_t *r16[4];

  Bus *bus;

} CPU;

typedef struct {
  const char *name;
  uint8_t cycles;
  void (*exec)(CPU *);
} Instruction;

// CPU initialization
void init_cpu(CPU *cpu, Bus *bus);

// Parsing opcodes
void execute(CPU *cpu);

void log_ins(CPU *cpu, Instruction *ins);

// Opcode decoding
static inline uint8_t op_x(uint8_t op) { return (op >> 6) & 0x4; }

static inline uint8_t op_y(uint8_t op) { return (op >> 3) & 0x7; }

static inline uint8_t op_z(uint8_t op) { return op & 0x7; }

// Memory reads
uint8_t read_n8(CPU *cpu);

uint8_t read_hl(CPU *cpu);

uint16_t read_n16(CPU *cpu);

// Memory writes
void write_hl(CPU *cpu, uint8_t val);

// Flags operations
void set_flag(CPU *cpu, Flag flag, bool val);

uint8_t get_flag(CPU *cpu, Flag flag);
