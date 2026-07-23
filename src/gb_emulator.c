#include "gb_emulator.h"
#include "cpu.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

bool emu_init(struct gb_emulator *gb, const char *path_to_rom, FILE *log_file) {
  if (!path_to_rom) {
    fprintf(stderr, "Could not start emulator, no ROM given.\n");
    return false;
  }

  return bus_init(&gb->bus, path_to_rom) &&
         cpu_init(&gb->cpu, &gb->bus, log_file);
}

void emu_tick(struct gb_emulator *gb) {
  const uint8_t cycles = cpu_tick(&gb->cpu);
  timer_tick(&gb->bus.timer, cycles, &gb->bus.interrupt);
}

void emu_close(struct gb_emulator *gb) { bus_close(&gb->bus); }
