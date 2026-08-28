#pragma once
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>

struct gameboy;
struct cpu;
struct instruction;

struct cpu_debugger {
  enum cpu_debugger_state {
    DEBUG_INIT,
    DEBUG_CONTINUE,
    DEBUG_BREAKPOINT
  } state;

  struct u16_stk breakpoints;
  struct u16_stk watches;
};

void cpu_debugger_create(struct cpu_debugger *dbg);
void cpu_debugger_destroy(struct cpu_debugger *dbg);

void cpu_debugger_print_cpu_step(const struct cpu_debugger *dbg,
                                 const struct cpu *cpu,
                                 const struct instruction *instr);
