#pragma once
#include "cpu_debugger.h"
#include "gameboy.h"
#include <stdio.h>

struct appstate {
  struct gameboy gameboy;
  FILE *log_file;
};

[[nodiscard]] struct appstate *appstate_malloc(int argc, char **argv);

void appstate_free(struct appstate *state);

void appstate_iterate(struct appstate *app);
