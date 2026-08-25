#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

struct gameboy;
struct cpu;

enum debugger_state { DEBUG_INIT, DEBUG_CONTINUE, DEBUG_BREAKPOINT };

struct u16_stk {
  uint16_t *data;
  size_t size;
  size_t capacity;
};

struct debugger {
  enum debugger_state state;
  struct u16_stk breakpoints;
  struct u16_stk watch_addresses;
};

void debugger_create(struct debugger *debugger);

void debugger_destroy(struct debugger *debugger);

void debugger_step(struct debugger *debugger, struct gameboy *gb,
                   FILE *log_file);
