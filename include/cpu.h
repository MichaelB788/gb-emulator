#pragma once
#include "cpu_debugger.h"
#include <stdint.h>

struct bus;
struct instruction;

static constexpr uint8_t FLAG_Z = 1 << 7;
static constexpr uint8_t FLAG_N = 1 << 6;
static constexpr uint8_t FLAG_H = 1 << 5;
static constexpr uint8_t FLAG_C = 1 << 4;

// The GameBoy's CPU
struct cpu {
  enum cpu_state {
    CPU_RUNNING,
    CPU_HALTED,
    CPU_STOPPED,
    CPU_DEBUGGING,
  } state;

  bool IME;
  bool halt_bug;
  bool ime_pending; // Setting IME has a delay
  bool debug_enabled;

  uint8_t IR; // Instruction register, holds the current opcode

  // 16-bit registers
  uint16_t PC;
  uint16_t SP;

  /**
   * 8-bit registers, of which can be combined to be interpreted as a 16-bit
   * register
   * NOTE: Ordering of 8-bit registers assumes the system is little endian
   */
  // clang-format off
  union { struct { uint8_t F, A; }; uint16_t AF; };
  union { struct { uint8_t C, B; }; uint16_t BC; };
  union { struct { uint8_t E, D; }; uint16_t DE; };
  union { struct { uint8_t L, H; }; uint16_t HL; };
  // clang-format on

  struct cpu_debugger debugger;

  struct bus *bus; // Non-owning pointer to bus, must not be NULL
};

void cpu_create(struct cpu *cpu, struct bus *bus);
void cpu_enable_debugging(struct cpu *cpu);
void cpu_destroy(struct cpu *cpu);

void cpu_step(struct cpu *cpu);

// Fetches the opcode in memory at PC and updates PC depending on the halt bug
[[nodiscard]] uint8_t cpu_fetch_next_opcode(struct cpu *cpu);

// Executes the given base instruction
void cpu_execute_instruction(struct cpu *cpu, const struct instruction *instr);

/// Sets the flag(s) to the given boolean value
void cpu_write_flags(struct cpu *cpu, uint8_t mask, bool val);

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
