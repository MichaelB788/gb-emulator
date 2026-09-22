#pragma once
#include <stdint.h>

struct bus;
struct instruction;

enum cpu_log_level { CPU_LOG_NONE, CPU_LOG_BRIEF, CPU_LOG_VERBOSE };
enum cpu_state { CPU_RUNNING, CPU_HALTED, CPU_HALT_BUG };

enum cpu_flags {
  FLAG_C = 1 << 4,
  FLAG_H = 1 << 5,
  FLAG_N = 1 << 6,
  FLAG_Z = 1 << 7
};

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

void cpu_write_flag(struct cpu *cpu, enum cpu_flags flag, bool val);

void cpu_step(struct cpu *cpu);

void cpu_execute(struct cpu *cpu, uint8_t opcode);
void cpu_execute_cb(struct cpu *cpu, uint8_t opcode);
