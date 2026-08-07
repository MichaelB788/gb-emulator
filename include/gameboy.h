#pragma once
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"

struct gameboy {
  struct bus bus;
  struct cpu cpu;
  struct cartridge cartridge;
};

bool gameboy_init(struct gameboy *gb, const char *path_to_rom);
void gameboy_quit(struct gameboy *gb);

void gameboy_step(struct gameboy *gb);
