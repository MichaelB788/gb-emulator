#pragma once
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct gameboy;
struct cpu;

struct debugger {
  bool debug_mode_active;
  struct u16_dynamic_vec breakpoints;
  struct u16_dynamic_vec watch_addresses;
};

bool debugger_create(struct debugger *debugger);
void debugger_destroy(struct debugger *debugger);

void debugger_check_for_breakpoints(struct debugger *debugger,
                                    const struct cpu *cpu);

void debugger_interactive_menu(struct debugger *debugger, struct gameboy *gb);
