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

bool appstate_init(struct appstate *app, char **argv);
void appstate_update(struct appstate *app);
void appstate_quit(struct appstate *state);
