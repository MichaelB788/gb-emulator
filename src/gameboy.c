#include "gameboy.h"
#include "cartridge.h"
#include "cpu.h"
#include <stdbool.h>

bool init_gameboy(GameBoy *gameboy, const char *path_to_rom) {
  gameboy->bus.cartridge = create_cartridge(path_to_rom);
  if (gameboy->bus.cartridge) {
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

void close_gameboy(GameBoy *gameboy) {
  if (gameboy) {
    destroy_cartridge(gameboy->bus.cartridge);
  }
}
