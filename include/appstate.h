#pragma once
#include "gameboy.h"
#include <stdio.h>

struct appstate {
  struct gameboy gb;
  FILE *log_file;
};

struct appstate *create_app(const char *path_to_rom, bool logging_enabled);

void destroy_app(struct appstate *state);

void log_curr_instr(struct appstate *state);

int cpu_step(struct appstate *state);
