#pragma once
#include "core/bus.h"
#include "core/cpu.h"

typedef struct {
  Bus bus;
  CPU cpu;
} GameBoy;

bool init_gameboy(GameBoy *gameboy, const char *path_to_rom);

void run_gameboy(GameBoy *gameboy);

void close_gameboy(GameBoy *gameboy);
