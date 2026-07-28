#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

struct bus;
struct interrupt;

#define FLAG_Z 7u
#define FLAG_N 6u
#define FLAG_H 5u
#define FLAG_C 4u

enum cpu_state { CPU_RUNNING, CPU_HALTED, CPU_STOPPED };

/**
 * The GameBoy's CPU
 */
struct cpu {
  union {
    struct {
      uint8_t B, C, D, E, H, L, F, A;
    };
    uint8_t r8[8];
  };

  uint16_t PC;
  uint16_t SP;

  bool IME;
  bool ime_pending;
  bool halt_bug;

  uint8_t IR;
  enum cpu_state state;

  struct bus *bus;
  struct interrupt *interrupt;
  FILE *log_file; // Observer, non-owning
};

bool cpu_init(struct cpu *cpu, struct bus *bus, FILE *log_file);
void cpu_step(struct cpu *cpu);

/// Register pair operations

uint16_t cpu_get_bc(const struct cpu *cpu);
uint16_t cpu_get_de(const struct cpu *cpu);
uint16_t cpu_get_hl(const struct cpu *cpu);
uint16_t cpu_get_af(const struct cpu *cpu);

void cpu_set_bc(struct cpu *cpu, uint16_t val);
void cpu_set_de(struct cpu *cpu, uint16_t val);
void cpu_set_hl(struct cpu *cpu, uint16_t val);
void cpu_set_af(struct cpu *cpu, uint16_t val);

/// Memory operations

// M-cycles: 1
uint8_t cpu_read_byte(struct cpu *cpu, uint16_t addr);

// M-cycles: 2
uint16_t cpu_read_word(struct cpu *cpu, uint16_t addr);

// M-cycles: 1
void cpu_write_byte(struct cpu *cpu, uint16_t addr, uint8_t val);

// M-cycles: 2
void cpu_write_word(struct cpu *cpu, uint16_t addr, uint16_t val);

// M-cycles: 0 untaken / 1 taken
void cpu_jump_a16(struct cpu *cpu, uint16_t addr, bool cond);

// M-cycles: 0 untaken / 3 taken
void cpu_call_a16(struct cpu *cpu, uint16_t addr, bool cond);

// M-cycles: 0 untaken / 3 taken
void cpu_return(struct cpu *cpu, bool cond);

/// Opcode dispatching

uint16_t cpu_get_r16(const struct cpu *cpu);
void cpu_set_r16(struct cpu *cpu, uint16_t val);

uint16_t cpu_get_r16stk(const struct cpu *cpu);
void cpu_set_r16stk(struct cpu *cpu, uint16_t val);

uint16_t cpu_get_r16mem(struct cpu *cpu);

bool cpu_test_cond(const struct cpu *cpu);
