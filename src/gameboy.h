#pragma once
#include "bus.h"
#include "cpu.h"
#include <stdbool.h>

struct gameboy {
  struct cpu cpu;
  struct bus bus;
};

bool gameboy_create(struct gameboy *gb, const char *rom_path);
void gameboy_destroy(struct gameboy *gb);

void gameboy_step(struct gameboy *gb);
