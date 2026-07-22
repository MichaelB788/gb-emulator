#pragma once
#include <stdbool.h>
#include <stdint.h>

struct bus;
struct interrupts;

#define FLAG_Z 7u
#define FLAG_N 6u
#define FLAG_H 5u
#define FLAG_C 4u

enum cpu_state { CPU_RUNNING, CPU_HALTED, CPU_STOPPED };

/**
 * The GameBoy's CPU
 */
struct cpu {
  uint8_t opcode;
  enum cpu_state state;

  union {
    struct {
      uint8_t B, C, D, E, H, L, F, A;
    };
    uint8_t r8[8];
  };

  bool IME;
  bool ime_pending;

  uint16_t PC;
  uint16_t SP;

  struct bus *bus;
};

void cpu_init(struct cpu *cpu, struct bus *bus);

uint8_t cpu_step(struct cpu *cpu);
uint8_t cpu_service_interrupts(struct cpu *cpu, struct interrupts *interrupt);

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

uint8_t cpu_fetch_n8(struct cpu *cpu);
uint16_t cpu_fetch_n16(struct cpu *cpu);

uint8_t cpu_read_hl(struct cpu *cpu);
void cpu_write_hl(struct cpu *cpu, uint8_t val);

void cpu_push_n16(struct cpu *cpu, uint16_t val);
uint16_t cpu_pop_n16(struct cpu *cpu);

/// Opcode dispatching

uint16_t cpu_get_r16(const struct cpu *cpu);
void cpu_set_r16(struct cpu *cpu, uint16_t val);

uint16_t cpu_get_r16stk(const struct cpu *cpu);
void cpu_set_r16stk(struct cpu *cpu, uint16_t val);

uint16_t cpu_get_r16mem(struct cpu *cpu);

bool cpu_test_cond(struct cpu *cpu);
