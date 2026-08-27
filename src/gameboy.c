#include "gameboy.h"
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#include "interrupts.h"
#include <assert.h>

bool gameboy_create(struct gameboy *gb, const char *path_to_rom) {
  if (cartridge_create(&gb->cart, path_to_rom)) {
    bus_init(&gb->bus, &gb->cart);
    cpu_init(&gb->cpu, &gb->bus);
    return true;
  }
  return false;
}

void gameboy_destroy(struct gameboy *gb) { cartridge_destroy(&gb->cart); }

void gameboy_step(struct gameboy *gb, FILE *log_file) {
  if (log_file) {
    cpu_log_state_reg16(&gb->cpu, log_file);
  }
  cpu_step(&gb->cpu);
  interrupts_service_pending(&gb->bus.interrupts, &gb->cpu);
}
