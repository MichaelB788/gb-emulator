#pragma once
#include <stdint.h>

struct bus;
struct instruction;

enum cpu_log_level { CPU_LOG_NONE, CPU_LOG_BRIEF, CPU_LOG_VERBOSE };
enum cpu_state { CPU_RUNNING, CPU_HALTED, CPU_HALT_BUG };
enum cpu_flags { FLAG_C = 0x10, FLAG_H = 0x20, FLAG_N = 0x40, FLAG_Z = 0x80 };

// The GameBoy's CPU
struct cpu {
  enum cpu_log_level log_level;
  enum cpu_state state;

  bool IME;
  bool ime_pending;

  // clang-format off
  union { struct { uint8_t F, A; }; uint16_t AF; };
  union { struct { uint8_t C, B; }; uint16_t BC; };
  union { struct { uint8_t E, D; }; uint16_t DE; };
  union { struct { uint8_t L, H; }; uint16_t HL; };
  // clang-format on

  uint16_t PC;
  uint16_t SP;

  struct bus *bus;
};

void cpu_init(struct cpu *cpu, struct bus *bus);

void cpu_step(struct cpu *cpu);

void cpu_execute(struct cpu *cpu, uint8_t opcode);
void cpu_execute_cb(struct cpu *cpu, uint8_t opcode);

void cpu_set_flag_as(struct cpu *cpu, enum cpu_flags flag, bool val);

// Memory operations

// M-cycles: 1
[[nodiscard]] uint8_t cpu_read_u8(const struct cpu *cpu, uint16_t addr);
// M-cycles: 2
[[nodiscard]] uint16_t cpu_read_u16(const struct cpu *cpu, uint16_t addr);
// M-cycles: 1
[[nodiscard]] uint8_t cpu_read_imm8(struct cpu *cpu);
// M-cycles: 2
[[nodiscard]] uint16_t cpu_read_imm16(struct cpu *cpu);

// M-cycles: 1
void cpu_write_u8(const struct cpu *cpu, uint16_t addr, uint8_t val);
// M-cycles: 2
void cpu_write_u16(const struct cpu *cpu, uint16_t addr, uint16_t val);

// M-cycles: 2
void cpu_push_u16(struct cpu *cpu, uint16_t u16);
// M-cycles: 2
uint16_t cpu_pop_u16(struct cpu *cpu);

// M-cycles: 0 untaken / 1 taken
void cpu_jump(struct cpu *cpu, uint16_t addr, bool cond);
// M-cycles: 0 untaken / 1 taken
void cpu_jump_rotation(struct cpu *cpu, int8_t offset, bool cond);
// M-cycles: 0 untaken / 3 taken
void cpu_call(struct cpu *cpu, uint16_t addr, bool cond);
// M-cycles: 0 untaken / 3 taken
void cpu_return(struct cpu *cpu, bool cond);
