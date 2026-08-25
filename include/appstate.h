#pragma once
#include "debugger.h"
#include "gameboy.h"

struct appstate {
  bool debug_enabled;

  struct gameboy gameboy;
  struct debugger debugger;

  FILE *log_file;
};

struct appstate *appstate_malloc(int argc, char **argv);

void appstate_free(struct appstate *state);

void appstate_iterate(struct appstate *app);
