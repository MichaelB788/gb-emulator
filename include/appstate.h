#pragma once
#include "debugger.h"
#include "gameboy.h"

struct appstate {
  struct gameboy gb;
  struct debugger debugger;
  FILE *log_file;
};

bool appstate_init(struct appstate *app, char **argv);
void appstate_update(struct appstate *app);
void appstate_quit(struct appstate *state);
