#pragma once
#include "app_result.h"
#include <stddef.h>
#include <stdint.h>

#define DBG_ENTRIES_MAX 20ul

enum cpu_dbg_state { CPU_DBG_INIT, CPU_DBG_INTERACTIVE, CPU_DBG_IDLE };

struct cpu;

struct cpu_dbg_u16_stk {
  uint16_t data[DBG_ENTRIES_MAX];
  size_t size;
};

struct cpu_dbg {
  enum cpu_dbg_state state;
  struct cpu_dbg_u16_stk breakpoints;
  struct cpu_dbg_u16_stk watchpoints;
  uint8_t u8_watched_buf[DBG_ENTRIES_MAX];
};

enum app_result cpu_dbg_step(struct cpu_dbg *dbg, struct cpu *cpu);
