#pragma once
#include "emulator.h"
#include <stdio.h>

struct appstate {
  struct emulator gb;
  FILE *log_file;
};

struct appstate *create_app(const char *path_to_rom, bool logging_enabled);

void destroy_app(struct appstate *state);
