#pragma once
#include <stdint.h>
#include <stdio.h>

struct bus;
struct instruction;
struct cpu_debugger;

static constexpr uint8_t FLAG_Z = 1 << 7;
static constexpr uint8_t FLAG_N = 1 << 6;
static constexpr uint8_t FLAG_H = 1 << 5;
static constexpr uint8_t FLAG_C = 1 << 4;

// The GameBoy's CPU
struct cpu {
  enum cpu_state { CPU_RUNNING, CPU_HALTED, CPU_STOPPED } state;
  uint8_t IR; // Instruction register, holds the current opcode

  bool IME;
  bool ime_pending; // Setting IME has a delay
  bool halt_bug;

  // clang-format off
  union { struct { uint8_t F, A; }; uint16_t AF; };
  union { struct { uint8_t C, B; }; uint16_t BC; };
  union { struct { uint8_t E, D; }; uint16_t DE; };
  union { struct { uint8_t L, H; }; uint16_t HL; };
  // clang-format on

  uint16_t PC;
  uint16_t SP;

  struct bus *bus;          // Non-owning pointer to bus, must not be NULL
  struct cpu_debugger *dbg; // Non-owning pointer to debugger, can be NULL
};

void cpu_init(struct cpu *cpu, struct bus *bus);

void cpu_step(struct cpu *cpu);

// Fetches the opcode in memory at PC and updates PC depending on the halt bug
[[nodiscard]] uint8_t cpu_fetch_next_opcode(struct cpu *cpu);

// Executes the given base instruction
void cpu_execute_instruction(struct cpu *cpu, struct instruction instr);

/// Sets the flag(s) to the given boolean value
void cpu_write_flags(struct cpu *cpu, uint8_t mask, bool val);

// Logging

void cpu_log_state_reg8(const struct cpu *cpu, FILE *output);
void cpu_log_state_reg16(const struct cpu *cpu, FILE *output);

// Memory operations

// M-cycles: 1
[[nodiscard]] uint8_t cpu_read_u8(const struct cpu *cpu, uint16_t addr);
// M-cycles: 2
[[nodiscard]] uint16_t cpu_read_u16(const struct cpu *cpu, uint16_t addr);
// M-cycles: 1
void cpu_write_u8(const struct cpu *cpu, uint16_t addr, uint8_t val);
// M-cycles: 2
void cpu_write_u16(const struct cpu *cpu, uint16_t addr, uint16_t val);

// M-cycles: 0 untaken / 1 taken
void cpu_jump(struct cpu *cpu, uint16_t addr, bool cond);
// M-cycles: 0 untaken / 1 taken
void cpu_jump_rotation(struct cpu *cpu, int8_t offset, bool cond);
// M-cycles: 0 untaken / 3 taken
void cpu_call(struct cpu *cpu, uint16_t addr, bool cond);
// M-cycles: 0 untaken / 3 taken
void cpu_return(struct cpu *cpu, bool cond);

// Opcode dispatching

/**
 * The GameBoy makes frequent use of the following bit pattern to decode
 * operands from instructions:
 *
 * [xx yyy zzz]
 *
 * In general:
 * - x: Used to determine the block the instruction lives in
 * - y: May determine an r8, r16, b3, tgt, or cond operand
 * - z: Used to determine an r8 operand
 *
 * More information can be found here:
 * https://gbdev.io/pandocs/CPU_Instruction_Set.html
 */

[[nodiscard]] uint8_t cpu_get_r8_y(const struct cpu *cpu);
[[nodiscard]] uint8_t cpu_get_r8_z(const struct cpu *cpu);

void cpu_set_r8_y(struct cpu *cpu, uint8_t val);
void cpu_set_r8_z(struct cpu *cpu, uint8_t val);

[[nodiscard]] uint16_t cpu_get_r16(const struct cpu *cpu);
[[nodiscard]] uint16_t cpu_get_r16stk(const struct cpu *cpu);
[[nodiscard]] uint16_t cpu_get_r16mem(struct cpu *cpu);

void cpu_set_r16(struct cpu *cpu, uint16_t val);
void cpu_set_r16stk(struct cpu *cpu, uint16_t val);

// Returns the result of a condition code: NZ, Z, NC, C
[[nodiscard]] bool cpu_cc(const struct cpu *cpu);
