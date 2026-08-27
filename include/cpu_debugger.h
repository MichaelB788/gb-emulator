#pragma once
#include "u16_stk.h"
#include <stddef.h>
#include <stdio.h>

struct gameboy;
struct cpu;

struct cpu_debugger {
  enum cpu_debugger_state {
    DEBUG_INIT,
    DEBUG_CONTINUE,
    DEBUG_BREAKPOINT
  } state;
  struct u16_stk breakpoints;
  struct u16_stk watch_addresses;
  FILE *log_file;
};

void cpu_debugger_create(struct cpu_debugger *dbg);

void cpu_debugger_destroy(struct cpu_debugger *dbg);

void cpu_debugger_step(struct cpu_debugger *dbg, struct gameboy *gb, FILE *out);
