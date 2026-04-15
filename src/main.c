#include "core/bus.h"
#include "core/cpu.h"
#include "core/gameboy.h"
#include <stddef.h>
#include <stdint.h>

int main(void) {
  GameBoy gameboy;
  init_gameboy(&gameboy);

  gameboy.cpu.B = 0x10;
  gameboy.cpu.C = 0x20;

  gameboy.cpu.bus->memory[0] = 0x41;
  gameboy.cpu.bus->memory[1] = 0x46;
  gameboy.cpu.bus->memory[2] = 0x70;
  gameboy.cpu.bus->memory[3] = 0x10;

  while (read_byte(&gameboy.bus, gameboy.cpu.PC) != 0x10)
    step(&gameboy.cpu);
}
