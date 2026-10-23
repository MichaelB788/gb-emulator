#pragma once
#include "app_result.h"
#include "bus.h"
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

  struct cpu cpu;
  struct cpu_dbg dbg;
  struct bus bus;
};

int gameboy_create(struct gameboy *gb, const char *rom_path,
                   enum gb_dbg_opt dbg_opt);
void gameboy_destroy(struct gameboy *gb);

enum app_result gameboy_step(struct gameboy *gb);
