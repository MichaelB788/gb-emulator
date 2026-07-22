#include "emulator.h"
#include "cartridge.h"
#include "cpu.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool emu_init(struct emulator *gb, const char *path_to_rom) {
  if (!path_to_rom) {
    fprintf(stderr, "Could not start emulator, no ROM given.\n");
    return false;
  }

  cpu_init(&gb->cpu, &gb->bus);
  return cart_init(&gb->bus.cartridge, path_to_rom);
}

void emu_close(struct emulator *gb) { cart_close(&gb->bus.cartridge); }
