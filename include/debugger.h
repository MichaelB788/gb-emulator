#pragma once
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct gameboy;
struct cpu;

enum debugger_state { DEBUG_INIT, DEBUG_CONTINUE, DEBUG_BREAKPOINT };

struct debugger {
  enum debugger_state state;
  struct u16_dynamic_vec breakpoints;
  struct u16_dynamic_vec watch_addresses;
};

bool create_debugger(struct debugger *debugger);
void destroy_debugger(struct debugger *debugger);

void debugger_step(struct debugger *debugger, struct gameboy *gb,
                   FILE *log_file);
