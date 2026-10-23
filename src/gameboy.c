#include "gameboy.h"
#include "app_result.h"
#include "bus.h"
#include "cpu.h"
#include "cpu_dbg.h"

int gameboy_create(struct gameboy *gb, const char *rom_path,
                   enum gb_dbg_opt dbg_opt) {
  gb->cpu = (struct cpu){.bus = &gb->bus, .PC = 0x100};
  gb->dbg = (struct cpu_dbg){0};

  switch (gb->opt = dbg_opt) {
  case GB_OPT_BRIEF_LOGGING:
    gb->cpu.log_level = CPU_LOG_BRIEF;
    break;
  case GB_OPT_VERBOSE_LOGGING:
    gb->cpu.log_level = CPU_LOG_VERBOSE;
    break;
  default:
    break;
  }

  return bus_create(&gb->bus, rom_path);
}

void gameboy_destroy(struct gameboy *gb) { bus_destroy(&gb->bus); }

enum app_result gameboy_step(struct gameboy *gb) {
  if (gb->opt == GB_OPT_INTERACTIVE_DEBUGGING)
    return cpu_dbg_step(&gb->dbg, &gb->cpu);
  else
    cpu_step(&gb->cpu);
  return APP_CONTINUE;
}
