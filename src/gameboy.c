#include "gameboy.h"
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#include "interrupts.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

bool gameboy_init(struct gameboy *gb, const char *path_to_rom) {
  if (path_to_rom && cartridge_create(&gb->cartridge, path_to_rom)) {
    return bus_init(&gb->bus, &gb->cartridge) && cpu_init(&gb->cpu, &gb->bus);
  } else {
    fprintf(stderr, "Could not initialize gameboy, invalid ROM\n");
    return false;
  }
}

void gameboy_quit(struct gameboy *gb) { cartridge_destroy(&gb->cartridge); }

void gameboy_step(struct gameboy *gb) {
  cpu_step(&gb->cpu);
  interrupts_service_pending(&gb->bus.interrupt, &gb->cpu);
}
