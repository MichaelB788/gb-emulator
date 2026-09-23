#pragma once
#include "bus.h"
#include "cartridge.h"
#include "cpu.h"
#include "cpu_dbg.h"

struct gameboy {
  enum gb_debug_option {
    GB_DEBUG_ENABLE_NONE = 0,
    GB_DEBUG_ENABLE_BREAKPOINTS,
    GB_DEBUG_ENABLE_LOG_BRIEF,
    GB_DEBUG_ENABLE_LOG_VERBOSE
  } debug_option;

  bool debug_enabled;

  struct cartridge cart;
  struct bus bus;
  struct cpu cpu;
  struct cpu_dbg dbg;
};

[[nodiscard]] bool gameboy_create(struct gameboy *gb, const char *path_to_rom,
                                  enum gb_debug_option dbg_opt);
void gameboy_destroy(struct gameboy *gb);

void gameboy_enable_breakpoints(struct gameboy *gb);
void gameboy_enable_logging_breif(struct gameboy *gb);
void gameboy_enable_logging_verbose(struct gameboy *gb);

void gameboy_step(struct gameboy *gb);
