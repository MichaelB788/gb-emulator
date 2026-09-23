#include "cpu_dbg.h"
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
static void cpu_dbg_interactive_menu(struct cpu_dbg *dbg, struct cpu *cpu,
                                     enum dbg_flags flags) {
  char input;
  printf("[%s%s%s(c)ontinue]: ", flags & DBG_STEP ? "(s)tep | " : "",
         flags & DBG_BREAKPOINT ? "(b)reakpoint | " : "",
         flags & DBG_WATCHPOINT ? "(w)atchpoint | " : "");
  scanf(" %c", &input);
  switch (input) {
  case 's':
    if (flags & DBG_STEP)
      cpu_step(cpu);
    else
      puts("Invalid option");
    break;
  case 'b':
    if (flags & DBG_BREAKPOINT)
      cpu_dbg_push_a16_input(&dbg->breakpoints);
    else
      puts("Invalid option");
    break;
  case 'w':
    if (flags & DBG_WATCHPOINT) {
      cpu_dbg_push_a16_input(&dbg->watchpoints);
      const size_t i = dbg->watchpoints.size - 1;
      dbg->watchpoint_memory[i] = bus_read(cpu->bus, dbg->watchpoints.data[i]);
    } else {
      puts("Invalid option");
    }
    break;
  case 'c':
    dbg->state = CPU_DBG_IDLE;
    cpu->log_level = CPU_LOG_NONE;
    break;
  default:
    break;
  }
}

static void cpu_dbg_observe_breakpoints(struct cpu_dbg *dbg, struct cpu *cpu) {
  if (u16_stk_contains(&dbg->breakpoints, cpu->PC)) {
    printf("\n"
           "Breakpoint hit: 0x%04X\n",
           cpu->PC);
    dbg->state = CPU_DBG_INTERACTIVE;
    cpu->log_level = CPU_LOG_VERBOSE;
  }
}

static void cpu_dbg_observe_watchpoints(struct cpu_dbg *dbg, struct cpu *cpu) {
  for (size_t i = 0; i < dbg->watchpoints.size; ++i) {
    const uint16_t u16 = bus_read(cpu->bus, dbg->watchpoints.data[i]);
    if (dbg->watchpoint_memory[i] != u16) {
      printf("\n"
             "Watchpoint 0x%04X changed: (0x%02X -> 0x%02X)\n",
             dbg->watchpoints.data[i], dbg->watchpoint_memory[i], u16);
      dbg->watchpoint_memory[i] = u16;
      dbg->state = CPU_DBG_INTERACTIVE;
      cpu->log_level = CPU_LOG_VERBOSE;
    }
  }
}

void cpu_dbg_step(struct cpu_dbg *dbg, struct cpu *cpu) {
  switch (dbg->state) {
  case CPU_DBG_INIT:
    cpu_dbg_interactive_menu(dbg, cpu, DBG_BREAKPOINT | DBG_WATCHPOINT);
    break;
  case CPU_DBG_INTERACTIVE:
    cpu_dbg_interactive_menu(dbg, cpu,
                             DBG_STEP | DBG_BREAKPOINT | DBG_WATCHPOINT);
    cpu_dbg_print_watchpoint_memory(dbg);
    break;
  case CPU_DBG_IDLE:
    cpu_dbg_observe_breakpoints(dbg, cpu);
    cpu_dbg_observe_watchpoints(dbg, cpu);

    cpu_step(cpu);

    // Breakpoint was hit or watchpoint was changed
    if (dbg->state == CPU_DBG_INTERACTIVE)
      cpu_dbg_print_watchpoint_memory(dbg);
    break;
  }
}
