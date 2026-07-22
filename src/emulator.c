#include "emulator.h"
#include "cpu.h"
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>

bool emu_init(struct emulator *gb, const char *path_to_rom) {
  if (!path_to_rom) {
    fprintf(stderr, "Could not start emulator, no ROM given.\n");
    return false;
  }

  return bus_init(&gb->bus, path_to_rom) && cpu_init(&gb->cpu, &gb->bus);
}

void emu_close(struct emulator *gb) { bus_close(&gb->bus); }
