#pragma once
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#include "cpu_debugger.h"

struct gameboy {
  struct cartridge cart;
  struct bus bus;
  struct cpu cpu;
  struct cpu_debugger dbg;
};

[[nodiscard]] bool gameboy_create(struct gameboy *gb, const char *path_to_rom);
void gameboy_destroy(struct gameboy *gb);

void gameboy_enable_debug(struct gameboy *gb);

void gameboy_step(struct gameboy *gb);
