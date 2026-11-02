#include "gameboy.h"
#include "bus.h"
#include "cpu.h"
#include <stdbool.h>

bool gameboy_create(struct gameboy *gb, const char *rom_path) {
  gb->cpu = (struct cpu){.bus = &gb->bus, .PC = 0x100};
  return bus_create(&gb->bus, rom_path);
}

void gameboy_destroy(struct gameboy *gb) { bus_destroy(&gb->bus); }

void gameboy_step(struct gameboy *gb) { cpu_step(&gb->cpu); }
