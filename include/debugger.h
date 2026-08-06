#pragma once
#include "dynamic_word_mem.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct debugger {
  struct dynamic_word_mem breakpoints;
  struct dynamic_word_mem watch_addresses;
};

bool debugger_init(struct debugger *debugger);

bool debugger_add_breakpoint(struct debugger *debugger, uint16_t addr);
bool debugger_add_watch_address(struct debugger *debugger, uint16_t addr);

void debugger_destroy(struct debugger *debugger);
