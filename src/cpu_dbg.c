#include "cpu_dbg.h"
#include "app_result.h"
#include "bus.h"
#include "cpu.h"
#include "instruction.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static void cpu_dbg_print_watchpoint_memory(const struct cpu_dbg *dbg) {
  if (dbg->watchpoints.size > 0) {
    static const size_t LEN = sizeof "[0000]:00 ";
    char wp_mem_str[dbg->watchpoints.size * LEN] = {};
    for (size_t i = 0; i < dbg->watchpoints.size; ++i) {
      char entry[LEN];
      snprintf(entry, LEN, "[%04X]:%02X ", dbg->watchpoints.data[i],
               dbg->u8_watched_buf[i]);
      strcat(wp_mem_str, entry);
    }
    printf("%s\n"
           "\n",
           wp_mem_str);
  }
}

static bool cpu_dbg_stk_contains(const struct cpu_dbg_u16_stk *stk,
                                 uint16_t u16) {
  for (size_t i = 0; i < stk->size; ++i) {
    if (stk->data[i] == u16)
      return true;
  }
  return false;
}

static void cpu_dbg_stk_insert_a16_stdin(struct cpu_dbg_u16_stk *stk) {
  if (stk->size == DBG_ENTRIES_MAX) {
    puts("Cannot insert any more addresses.");
    return;
  }

  uint16_t a16;
  printf("Address: ");
  scanf("%hx", &a16);
  getchar();

  if (cpu_dbg_stk_contains(stk, a16)) {
    puts("Address already exists.");
    return;
  }

  stk->data[stk->size++] = a16;
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
      cpu_dbg_stk_insert_a16_stdin(&dbg->breakpoints);
    }
    return APP_CONTINUE;
  case 'w':
    if (flags & DBG_WATCHPOINT) {
      getchar();
      cpu_dbg_stk_insert_a16_stdin(&dbg->watchpoints);
      const size_t i = dbg->watchpoints.size;
      dbg->u8_watched_buf[i] = bus_read(cpu->bus, dbg->watchpoints.data[i]);
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
    if (cpu_dbg_stk_contains(&dbg->breakpoints, cpu->PC)) {
      printf("\n"
             "Breakpoint hit: 0x%04X\n",
             cpu->PC);
      should_break = true;
    }
    for (size_t i = 0; i < dbg->watchpoints.size; ++i) {
      const uint8_t u8 = bus_read(cpu->bus, dbg->watchpoints.data[i]);
      if (dbg->u8_watched_buf[i] != u8) {
        printf("\n"
               "Watchpoint 0x%04X changed: (0x%02X -> 0x%02X)\n",
               dbg->watchpoints.data[i], dbg->u8_watched_buf[i], u8);
        dbg->u8_watched_buf[i] = u8;
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
