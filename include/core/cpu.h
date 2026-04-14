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

  uint8_t A, F;

  uint16_t PC, SP;

  Bus *bus;
} CPU;

inline void set_flag(CPU *cpu, Flag flag) { set_bit(&cpu->F, (uint8_t)flag); }
