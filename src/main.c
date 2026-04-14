#include "core/bus.h"
#include "core/cpu.h"
#include "core/gameboy.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(void) {
  GameBoy gameboy;
  init_gameboy(&gameboy);

  gameboy.cpu.BC.byte.hi = 0x10;
  gameboy.cpu.BC.byte.lo = 0x20;

  gameboy.cpu.bus->memory[0] = 0x41;
  gameboy.cpu.bus->memory[1] = 0x46;

  for (size_t i = 0; i < 3; ++i) {
    execute(&gameboy.cpu);
  }
}
