#pragma once
#include "bus.h"
#include "cpu.h"

struct gameboy {
  struct bus bus;
  struct cpu cpu;
};

bool create_gameboy(struct gameboy *gb, const char *path_to_rom);
void destroy_gameboy(struct gameboy *gb);

void gameboy_step(struct gameboy *gb, FILE *log_file);
