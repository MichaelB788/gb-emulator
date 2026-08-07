#pragma once
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct cpu;

struct debugger {
  struct u16_dynamic_vec breakpoints;
  struct u16_dynamic_vec watch_addresses;
};

bool debugger_create(struct debugger *debugger);
void debugger_destroy(struct debugger *debugger);

void debugger_step(struct debugger *debugger, struct cpu *cpu);
// TODO: Allow user to edit debugger variables through the terminal
