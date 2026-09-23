#pragma once
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>

struct cpu;

enum cpu_debugger_state {
  CPU_DEBUGGER_INIT,
  CPU_DEBUGGER_INTERACTIVE,
  CPU_DEBUGGER_IDLE
};

struct cpu_debugger {
  enum cpu_debugger_state state;

  struct u16_stk breakpoints;

  struct u16_stk watchpoints;
  uint16_t watched_memory[U16_STK_CAPACITY];
};

void cpu_debugger_init(struct cpu_debugger *dbg);

void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu);
