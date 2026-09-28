#pragma once
#include "app_result.h"
#include <stddef.h>
#include <stdint.h>

struct cpu;

enum cpu_dbg_state { CPU_DBG_INIT, CPU_DBG_INTERACTIVE, CPU_DBG_IDLE };

static constexpr size_t DBG_ENTRIES_MAX = 20;

struct cpu_dbg {
  enum cpu_dbg_state state;

  uint16_t breakpoints[DBG_ENTRIES_MAX];
  size_t breakpoints_size;

  uint16_t watchpoints[DBG_ENTRIES_MAX];
  uint16_t watchpoint_memory[DBG_ENTRIES_MAX];
  size_t watchpoints_size;
};

void cpu_dbg_init(struct cpu_dbg *dbg);

[[nodiscard]] enum app_result cpu_dbg_step(struct cpu_dbg *dbg,
                                           struct cpu *cpu);
