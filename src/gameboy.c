#include "gameboy.h"
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#include "cpu_debugger.h"
#include <assert.h>

bool gameboy_create(struct gameboy *gb, const char *path_to_rom) {
  if (cartridge_create(&gb->cart, path_to_rom)) {
    bus_init(&gb->bus, &gb->cart);
    cpu_init(&gb->cpu, &gb->bus);
    return true;
  }
  return false;
}

void gameboy_destroy(struct gameboy *gb) {
  cartridge_destroy(&gb->cart);
  cpu_debugger_destroy(&gb->dbg);
}

void gameboy_enable_debug(struct gameboy *gb) {
  cpu_debugger_create(&gb->dbg);
  gb->cpu.dbg = &gb->dbg;
}

void gameboy_step(struct gameboy *gb) { cpu_step(&gb->cpu); }
