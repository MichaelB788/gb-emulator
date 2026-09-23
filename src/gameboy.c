#include "gameboy.h"
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#include "cpu_dbg.h"
#include <assert.h>

bool gameboy_create(struct gameboy *gb, const char *path_to_rom,
                    enum gb_dbg_opt dbg_opt) {
  if (!cartridge_create(&gb->cart, path_to_rom))
    return false;

  bus_init(&gb->bus, &gb->cart);
  cpu_init(&gb->cpu, &gb->bus);

  switch (gb->opt = dbg_opt) {
  case GB_OPT_NONE:
    break;
  case GB_OPT_INTERACTIVE_DEBUGGING:
    cpu_dbg_init(&gb->dbg);
    break;
  case GB_OPT_BRIEF_LOGGING:
    gb->cpu.log_level = CPU_LOG_BRIEF;
    break;
  case GB_OPT_VERBOSE_LOGGING:
    gb->cpu.log_level = CPU_LOG_VERBOSE;
    break;
  }

  return true;
}

void gameboy_enable_breakpoints(struct gameboy *gb) {}

void gameboy_destroy(struct gameboy *gb) { cartridge_destroy(&gb->cart); }

void gameboy_step(struct gameboy *gb) {
  if (gb->opt == GB_OPT_INTERACTIVE_DEBUGGING)
    cpu_dbg_step(&gb->dbg, &gb->cpu);
  else
    cpu_step(&gb->cpu);
}
