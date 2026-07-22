#pragma once
#include "bus.h"
#include "cpu.h"
#include <stdint.h>

struct emulator {
  struct bus bus;
  struct cpu cpu;
};

bool emu_init(struct emulator *gb, const char *path_to_rom);
void emu_close(struct emulator *gb);
