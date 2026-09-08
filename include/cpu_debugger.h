#pragma once
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>

struct gameboy;
struct cpu;
struct instruction;

struct cpu_debugger {
  enum cpu_debugger_state {
    CPU_DEBUGGER_INACTIVE,
    CPU_DEBUGGER_BREAKPOINT_HIT,
    CPU_DEBUGGER_LOGGING
  } state;
  struct u16_stk breakpoints;
  struct u16_stk watches;
};

void cpu_debugger_create(struct cpu_debugger *dbg);
void cpu_debugger_destroy(struct cpu_debugger *dbg);

void cpu_debugger_print_cpu_step(const struct cpu_debugger *dbg,
                                 const struct cpu *cpu,
                                 const struct instruction *instr);

void cpu_debugger_init(struct cpu_debugger *dbg);
void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu,
                       const struct instruction *instr);

void cpu_debugger_check_for_breakpoints(struct cpu_debugger *dbg, uint16_t pc);
