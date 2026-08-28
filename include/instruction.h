#pragma once
#include <stdint.h>

struct cpu;

typedef void (*cpu_instr_fp)(struct cpu *);

struct instruction {
  uint8_t opcode;
  cpu_instr_fp handler;
  const char *mnemonic;
};
