#pragma once
#include "bus.h"
#include "cpu.h"

struct gb_emulator {
  struct bus bus;
  struct cpu cpu;
};

bool emu_init(struct gb_emulator *gb, const char *path_to_rom);
void emu_tick(struct gb_emulator *gb);
void emu_close(struct gb_emulator *gb);
