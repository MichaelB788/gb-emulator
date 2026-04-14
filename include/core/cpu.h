#pragma once
#include "core/bus.h"
#include "util/bitwise.h"
#include <stdint.h>

typedef enum { CARRY = 4, HALF_CARRY, SUBTRACTION, ZERO } Flag;

typedef union {
  struct {
    uint8_t lo, hi;
  } byte;
  uint16_t word;
} RegisterPair;

typedef struct {
  RegisterPair BC, DE, HL;

  uint8_t A, F, IR;

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
void init_cpu(CPU *cpu);

// Opcode decoding
static inline uint8_t op_x(uint8_t op) { return (op >> 6) & 0x4; }

static inline uint8_t op_y(uint8_t op) { return (op >> 3) & 0x7; }

static inline uint8_t op_z(uint8_t op) { return op & 0x7; }

// Memory reads
static inline uint8_t read_n8(CPU *cpu) {
  return read_byte(cpu->bus, cpu->PC++);
}

static inline uint8_t read_hl(CPU *cpu) {
  return read_byte(cpu->bus, cpu->HL.word);
}

static inline uint16_t read_n16(CPU *cpu) {
  uint8_t lo = read_byte(cpu->bus, cpu->PC++);
  uint8_t hi = read_byte(cpu->bus, cpu->PC++);
  return (uint16_t)hi << 8 | lo;
}

// Memory writes
static inline void write_hl(CPU *cpu, uint8_t val) {
  write_byte(cpu->bus, cpu->HL.word, val);
}

// Flags operations
static inline void set_flag(CPU *cpu, Flag flag) {
  set_bit(&cpu->F, (uint8_t)flag);
}

static inline void clear_flag(CPU *cpu, Flag flag) {
  clear_bit(&cpu->F, (uint8_t)flag);
}

static inline uint8_t get_flag(CPU *cpu, Flag flag) {
  return get_bit(cpu->F, (uint8_t)flag);
}
