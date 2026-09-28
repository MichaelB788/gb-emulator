#include "cpu_dbg.h"
#include "app_result.h"
#include "bus.h"
#include "cpu.h"
#include "instruction.h"
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void cpu_dbg_init(struct cpu_dbg *dbg) {
  dbg->state = CPU_DBG_INIT;
  dbg->breakpoints.size = dbg->watchpoints.size = 0;
}

static void cpu_dbg_push_a16_input(struct u16_stk *out) {
  uint16_t address;
  printf("Address: ");
  scanf("%hx", &address);
  getchar();
  if (!u16_stk_contains(out, address)) {
    if (!u16_stk_push(out, address))
      fprintf(stderr, "Stack at max capacity. Value discarded.\n");
  }
}

static void cpu_dbg_print_watchpoint_memory(const struct cpu_dbg *dbg) {
  if (dbg->watchpoints.size > 0) {
    static constexpr size_t LEN = sizeof "[0000]:00 ";
    char wp_mem_str[dbg->watchpoints.size * LEN] = {};
    for (size_t i = 0; i < dbg->watchpoints.size; ++i) {
      char entry[LEN];
      snprintf(entry, LEN, "[%04X]:%02X ", dbg->watchpoints.data[i],
               dbg->watchpoint_memory[i]);
      strcat(wp_mem_str, entry);
    }
    printf("%s\n"
           "\n",
           wp_mem_str);
  }
}

enum dbg_flags {
  // clang-format off
  DBG_WATCHPOINT = 1 << 0,
  DBG_BREAKPOINT = 1 << 1,
  DBG_STEP       = 1 << 2
  // clang-format on
};

static enum app_result cpu_dbg_interactive_menu(struct cpu_dbg *dbg,
                                                struct cpu *cpu,
                                                enum dbg_flags flags) {
  printf("[%s%s%s(c)ontinue | (q)uit]: ", flags & DBG_STEP ? "(s)tep | " : "",
         flags & DBG_BREAKPOINT ? "(b)reakpoint | " : "",
         flags & DBG_WATCHPOINT ? "(w)atchpoint | " : "");
  switch (getchar()) {
  case 's':
    if (flags & DBG_STEP) {
      getchar();
      return cpu_step(cpu);
    }
    return APP_CONTINUE;
  case 'b':
    if (flags & DBG_BREAKPOINT) {
      getchar();
      cpu_dbg_push_a16_input(&dbg->breakpoints);
    }
    return APP_CONTINUE;
  case 'w':
    if (flags & DBG_WATCHPOINT) {
      getchar();
      cpu_dbg_push_a16_input(&dbg->watchpoints);
      const size_t i = dbg->watchpoints.size - 1;
      dbg->watchpoint_memory[i] = bus_read(cpu->bus, dbg->watchpoints.data[i]);
    }
    return APP_CONTINUE;
  case 'c':
    getchar();
    dbg->state = CPU_DBG_IDLE;
    cpu->log_level = CPU_LOG_NONE;
    return APP_CONTINUE;
  case 'q':
  case EOF:
    return APP_SUCCESS;
  default:
    return APP_CONTINUE;
  }
}

enum app_result cpu_dbg_step(struct cpu_dbg *dbg, struct cpu *cpu) {
  enum app_result res;
  switch (dbg->state) {
  case CPU_DBG_INIT:
    return cpu_dbg_interactive_menu(dbg, cpu, DBG_BREAKPOINT | DBG_WATCHPOINT);
  case CPU_DBG_INTERACTIVE:
    res = cpu_dbg_interactive_menu(dbg, cpu,
                                   DBG_STEP | DBG_BREAKPOINT | DBG_WATCHPOINT);
    if (res == APP_CONTINUE)
      cpu_dbg_print_watchpoint_memory(dbg);

    return res;
  case CPU_DBG_IDLE: {
    bool should_break = false;

    // Check breakpoints
    if (u16_stk_contains(&dbg->breakpoints, cpu->PC)) {
      printf("\n"
             "Breakpoint hit: 0x%04X\n",
             cpu->PC);
      should_break = true;
    }

    // Check watchpoints
    for (size_t i = 0; i < dbg->watchpoints.size; ++i) {
      const uint16_t u16 = bus_read(cpu->bus, dbg->watchpoints.data[i]);
      if (dbg->watchpoint_memory[i] != u16) {
        printf("\n"
               "Watchpoint 0x%04X changed: (0x%02X -> 0x%02X)\n",
               dbg->watchpoints.data[i], dbg->watchpoint_memory[i], u16);
        dbg->watchpoint_memory[i] = u16;
        should_break = true;
      }
    }

    if (should_break) {
      dbg->state = CPU_DBG_INTERACTIVE;
      cpu->log_level = CPU_LOG_VERBOSE;
      if ((res = cpu_step(cpu)) == APP_CONTINUE)
        cpu_dbg_print_watchpoint_memory(dbg);
      return res;
    }

    return cpu_step(cpu);
  }
  }
}
