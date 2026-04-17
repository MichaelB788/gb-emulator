#pragma once
#include "core/bus.h"
#include "core/cartridge.h"
#include "core/cpu.h"

typedef struct {
  Cartridge cartridge;
  Bus bus;
  CPU cpu;
} GameBoy;

void init_gameboy(GameBoy *gameboy, const char *path_to_rom);

void run_gameboy(GameBoy *gameboy);

void close_gameboy(GameBoy *gameboy);
