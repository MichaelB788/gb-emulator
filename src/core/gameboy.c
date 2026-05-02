#include "core/gameboy.h"
#include "core/cartridge.h"
#include "core/cpu.h"
#include <stdbool.h>

bool init_gameboy(GameBoy *gameboy, const char *path_to_rom) {
  gameboy->cartridge = create_cartridge(path_to_rom);
  if (gameboy->cartridge) {
    gameboy->bus.cartridge = gameboy->cartridge;
    gameboy->bus.interrupt_enable = false;
    init_cpu(&gameboy->cpu, &gameboy->bus);
    return true;
  } else {
    gameboy->cpu.state = CPU_STOPPED;
    return false;
  }
}

void run_gameboy(GameBoy *gameboy) {
  while (gameboy->cpu.state == CPU_RUNNING) {
    step(&gameboy->cpu);
  }
}

void close_gameboy(GameBoy *gameboy) { destroy_cartridge(gameboy->cartridge); }
