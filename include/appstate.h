#pragma once
#include "cpu_debugger.h"
#include "gameboy.h"
#include <stdio.h>

struct appstate {
  bool debug_enabled;

  struct gameboy gameboy;
  struct cpu_debugger cpu_dbg;

  FILE *log_file;
};

[[nodiscard]] struct appstate *appstate_malloc(int argc, char **argv);

void appstate_free(struct appstate *state);

void appstate_iterate(struct appstate *app);
