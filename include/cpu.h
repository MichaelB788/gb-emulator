#pragma once
#include <stdbool.h>
#include <stdint.h>

struct bus;
struct instruction;

enum cpu_log_level { CPU_LOG_NONE, CPU_LOG_BRIEF, CPU_LOG_VERBOSE };
enum cpu_state { CPU_RUNNING, CPU_HALTED, CPU_HALT_BUG, CPU_STOPPED };

enum cpu_flags {
  FLAG_Z = 1 << 7,
  FLAG_N = 1 << 6,
  FLAG_H = 1 << 5,
  FLAG_C = 1 << 4
};

// The GameBoy's CPU
struct cpu {
  enum cpu_log_level log_level;
  enum cpu_state state;

  bool IME;         // interrupt master enable
  bool ime_pending; // setting ime has a delay of one instruction

  uint8_t A, F; // 8-bit register pair: accumulator and flags
  uint8_t B, C; // 8-bit register pair: BC
  uint8_t D, E; // 8-bit register pair: DE
  uint8_t H, L; // 8-bit register pair: HL

  uint16_t PC; // 16-bit register address: progrm counter
  uint16_t SP; // 16-bit register address: stack pointer

  struct bus *bus;
};

uint16_t cpu_get_af(const struct cpu *cpu);
uint16_t cpu_get_bc(const struct cpu *cpu);
uint16_t cpu_get_de(const struct cpu *cpu);
uint16_t cpu_get_hl(const struct cpu *cpu);

void cpu_set_af(struct cpu *cpu, uint16_t u16);
void cpu_set_bc(struct cpu *cpu, uint16_t u16);
void cpu_set_de(struct cpu *cpu, uint16_t u16);
void cpu_set_hl(struct cpu *cpu, uint16_t u16);

void cpu_log_step_brief(const struct cpu *cpu);
void cpu_log_step_verbose(const struct cpu *cpu, uint8_t opcode,
                          const char *mnemonic);

void cpu_step(struct cpu *cpu);

void cpu_execute(struct cpu *cpu, uint8_t opcode);
void cpu_execute_cb(struct cpu *cpu, uint8_t opcode);
