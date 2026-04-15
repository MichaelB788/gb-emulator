#pragma once
#include "core/bus.h"
#include "util/bitwise.h"
#include <assert.h>
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

  uint8_t A, F, opcode, cycles_taken;

  uint16_t PC, SP;

  bool IME;

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
uint8_t step(CPU *cpu);

void log_ins(CPU *cpu, Instruction *ins);

// Opcode decoding
static inline uint8_t op_y(uint8_t op) { return (op >> 3) & 0x7; }

static inline uint8_t op_z(uint8_t op) { return op & 0x7; }

static inline uint8_t *r8(CPU *cpu) {
  uint8_t i = op_z(cpu->opcode);
  assert(i != 6);
  return cpu->r8[i];
}

static inline uint8_t *r8_dest(CPU *cpu) {
  uint8_t i = op_y(cpu->opcode);
  assert(i != 6);
  return cpu->r8[i];
}

static inline uint16_t *r16(CPU *cpu) {
  return cpu->r16[op_y(cpu->opcode) >> 1];
}

bool check_cc(CPU *cpu);

// Memory reads
static inline uint8_t read_n8(CPU *cpu) {
  return read_byte(cpu->bus, cpu->PC++);
}

static inline uint8_t read_hl(CPU *cpu) {
  return read_byte(cpu->bus, cpu->HL.word);
}

uint16_t read_n16(CPU *cpu);

// Memory writes
static inline void write_hl(CPU *cpu, uint8_t val) {
  write_byte(cpu->bus, cpu->HL.word, val);
}

// Flags operations
static inline void set_flag(CPU *cpu, Flag flag, bool val) {
  set_bit(&cpu->F, (uint8_t)flag, val);
}

static inline bool get_flag(CPU *cpu, Flag flag) {
  return get_bit(cpu->F, (uint8_t)flag);
}
