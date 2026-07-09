#pragma once
#include "gameboy.h"
#include <stdio.h>

struct appstate {
  struct gameboy gb;
  FILE *log_file;
};

void log_curr_instr(struct appstate *state);

int cpu_step(struct appstate *state);
