#include "core/gameboy.h"
#include "core/bus.h"
#include "core/cartridge.h"
#include "core/cpu.h"
#include <stdbool.h>
#include <stdio.h>

void init_gameboy(GameBoy *gameboy, const char *path_to_rom) {
  if (init_cartridge(&gameboy->cartridge, path_to_rom)) {
    gameboy->bus.cartridge = &gameboy->cartridge;
    init_cpu(&gameboy->cpu, &gameboy->bus);
  } else {
    gameboy->cpu.running = false;
    fprintf(stderr, "Could not start GameBoy");
  }
}

void run_gameboy(GameBoy *gameboy) {
  while (gameboy->cpu.running)
    step(&gameboy->cpu);
}

void close_gameboy(GameBoy *gameboy) { close_cartridge(&gameboy->cartridge); }
