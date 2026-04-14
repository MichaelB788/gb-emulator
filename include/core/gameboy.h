#pragma once
#include "core/bus.h"
#include "core/cpu.h"

typedef struct {
  Bus bus;
  CPU cpu;
} GameBoy;

void init_gameboy(GameBoy *gameboy);
