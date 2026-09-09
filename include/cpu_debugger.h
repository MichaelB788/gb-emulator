#pragma once
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>

struct cpu;

struct cpu_debugger {
  enum cpu_debugger_state {
    CPU_DEBUGGER_INIT,
    CPU_DEBUGGER_WATCH_FOR_BREAKPOINTS,
    CPU_DEBUGGER_BREAKPOINT_HIT
  } state;

  struct u16_stk breakpoints;
};

void cpu_debugger_create(struct cpu_debugger *dbg);
void cpu_debugger_destroy(struct cpu_debugger *dbg);

void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu);
