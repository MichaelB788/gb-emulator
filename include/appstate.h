#pragma once
#include "debugger.h"
#include "gameboy.h"
#include <stdbool.h>

struct appstate {
  struct gameboy gb;

  bool debug_enabled;
  struct debugger debugger;

  FILE *log_file;
};

struct appstate *create_appstate(int argc, char **argv);

void destroy_appstate(struct appstate *state);

void appstate_iterate(struct appstate *app);
