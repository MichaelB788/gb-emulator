#include "core/gameboy.h"
#include "core/cpu.h"

void init_gameboy(GameBoy *gameboy) { init_cpu(&gameboy->cpu, &gameboy->bus); }
