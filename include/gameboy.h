#pragma once
#include "app_result.h"
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#include "cpu_dbg.h"

enum gb_dbg_opt {
  GB_OPT_NONE,
  GB_OPT_INTERACTIVE_DEBUGGING,
  GB_OPT_BRIEF_LOGGING,
  GB_OPT_VERBOSE_LOGGING
};

struct gameboy {
  enum gb_dbg_opt opt;

  struct cartridge cart;
  struct bus bus;
  struct cpu cpu;
  struct cpu_dbg dbg;
};

[[nodiscard]] bool gameboy_create(struct gameboy *gb, const char *path_to_rom,
                                  enum gb_dbg_opt dbg_opt);
void gameboy_destroy(struct gameboy *gb);

[[nodiscard]] enum app_result gameboy_step(struct gameboy *gb);
