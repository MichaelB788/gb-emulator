#pragma once
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct gameboy;
struct cpu;

enum debugger_state { DEBUG_CONTINUE, DEBUG_BREAKPOINT, DEBUG_INIT };

struct debugger {
  enum debugger_state state;
  struct u16_dynamic_vec breakpoints;
  struct u16_dynamic_vec watch_addresses;
};

bool debugger_has_init(struct debugger *debugger);

struct debugger create_debugger();

void destroy_debugger(struct debugger *debugger);

void debugger_check_for_breakpoints(struct debugger *debugger,
                                    const struct cpu *cpu);

void debugger_initialize_variables_menu(struct debugger *debugger);

void debugger_breakpoint_menu(struct debugger *debugger, struct gameboy *gb);
