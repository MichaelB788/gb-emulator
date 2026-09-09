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

void gameboy_enable_debugging(struct gameboy *gb) {
  gb->debug_enabled = true;
  cpu_debugger_create(&gb->dbg);
}

void gameboy_destroy(struct gameboy *gb) {
  cartridge_destroy(&gb->cart);
  cpu_debugger_destroy(&gb->dbg);
}

void gameboy_step(struct gameboy *gb) {
  if (gb->debug_enabled)
    cpu_debugger_step(&gb->dbg, &gb->cpu);
  else
    cpu_step(&gb->cpu);
}
