#include "gameboy.h"
#include "bus.h"
#include "cpu.h"
#include "interrupts.h"
#include <assert.h>
#include <stdbool.h>

bool create_gameboy(struct gameboy *gb, const char *path_to_rom) {
  if (create_bus(&gb->bus, path_to_rom)) {
    gb->cpu = create_cpu(&gb->bus);
    return true;
  }
  return false;
}

void destroy_gameboy(struct gameboy *gb) { destroy_bus(&gb->bus); }

void gameboy_step(struct gameboy *gb) {
  cpu_step(&gb->cpu);
  interrupts_service_pending(&gb->bus.interrupts, &gb->cpu);
}
