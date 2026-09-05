#pragma once
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>

struct gameboy;
struct cpu;
struct instruction;

enum res { CPU_DEBUG_WAIT, CPU_DEBUG_STEP, CPU_CONTINUE };

struct cpu_debugger {
  enum cpu_debug_state {
    CPU_DEBUG_INIT,
    CPU_DEBUG_BREAKPOINT_HIT,
  } state;

  struct u16_stk breakpoints;
  struct u16_stk watches;
};

void cpu_debugger_create(struct cpu_debugger *dbg);
void cpu_debugger_destroy(struct cpu_debugger *dbg);

void cpu_debugger_print_cpu_step(const struct cpu_debugger *dbg,
                                 const struct cpu *cpu,
                                 const struct instruction *instr);

[[nodiscard]] enum res cpu_debugger_step(struct cpu_debugger *dbg);

[[nodiscard]] bool
cpu_debugger_was_breakpoint_hit(const struct cpu_debugger *dbg, uint16_t pc);
