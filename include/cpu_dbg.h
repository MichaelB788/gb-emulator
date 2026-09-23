#pragma once
#include "u16_stk.h"
#include <stdint.h>

struct cpu;

enum cpu_dbg_state { CPU_DBG_INIT, CPU_DBG_INTERACTIVE, CPU_DBG_IDLE };

struct cpu_dbg {
  enum cpu_dbg_state state;

  struct u16_stk breakpoints;

  struct u16_stk watchpoints;
  uint16_t watchpoint_memory[U16_STK_CAPACITY];
};

void cpu_dbg_init(struct cpu_dbg *dbg);

void cpu_dbg_step(struct cpu_dbg *dbg, struct cpu *cpu);
