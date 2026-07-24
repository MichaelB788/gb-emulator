#pragma once
#include "bus.h"
#include "cpu.h"

struct gb_emulator {
  struct bus bus;
  struct cpu cpu;
};

bool gameboy_init(struct gb_emulator *gb, const char *path_to_rom,
                  FILE *log_file);
void gameboy_update(struct gb_emulator *gb);
void gameboy_close(struct gb_emulator *gb);
