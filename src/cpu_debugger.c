#include "cpu_debugger.h"
#include "bus.h"
#include "cpu.h"
#include "instruction.h"
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Pushes a u16 address to `out` via terminal input
static void u16_stk_push_a16_input(struct u16_stk *out) {
  uint16_t address;
  printf("Address: ");
  scanf("%hx", &address);
  if (!u16_stk_contains(out, address)) {
    if (!u16_stk_push(out, address))
      fprintf(stderr, "Stack at max capacity. Value discarded.\n");
  }
}

static void cpu_debugger_print_watch_addresses(const struct cpu_debugger *dbg,
                                               const struct bus *bus) {
  static constexpr size_t WATCH_ENTRY_SIZE = sizeof "[0000]:00 ";

  if (dbg->watchpoints.size > 0) {
    char buffer[dbg->watchpoints.size * WATCH_ENTRY_SIZE] = {};
    for (size_t i = 0; i < dbg->watchpoints.size; ++i) {
      const uint16_t addr = dbg->watchpoints.data[i];
      char entry[WATCH_ENTRY_SIZE];
      snprintf(entry, WATCH_ENTRY_SIZE, "[%04X]:%02X ", addr,
               bus_read(bus, addr));
      strcat(buffer, entry);
    }
    printf("%s\n", buffer);
  }
}

void cpu_debugger_init(struct cpu_debugger *dbg) {
  dbg->state = CPU_DEBUGGER_INIT;
  dbg->breakpoints.size = dbg->watchpoints.size = 0;
}

enum dbg_flags {
  // clang-format off
  DBG_WATCHPOINT = 1 << 0,
  DBG_BREAKPOINT = 1 << 1,
  DBG_STEP       = 1 << 2
  // clang-format on
};

static void cpu_debugger_interactive_menu(struct cpu_debugger *dbg,
                                          struct cpu *cpu,
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
      u16_stk_push_a16_input(&dbg->breakpoints);
    else
      puts("Invalid option");
    break;
  case 'w':
    if (flags & DBG_WATCHPOINT) {
      u16_stk_push_a16_input(&dbg->watchpoints);
      const size_t i = dbg->watchpoints.size - 1;
      dbg->watched_memory[i] = bus_read(cpu->bus, dbg->watchpoints.data[i]);
    } else {
      puts("Invalid option");
    }
    break;
  case 'c':
    dbg->state = CPU_DEBUGGER_IDLE;
    cpu->log_level = CPU_LOG_NONE;
    break;
  }
}

void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu) {
  switch (dbg->state) {
  case CPU_DEBUGGER_INIT:
    cpu_debugger_interactive_menu(dbg, cpu, DBG_BREAKPOINT | DBG_WATCHPOINT);
    break;
  case CPU_DEBUGGER_INTERACTIVE:
    cpu_debugger_interactive_menu(dbg, cpu,
                                  DBG_STEP | DBG_BREAKPOINT | DBG_WATCHPOINT);
    break;
  case CPU_DEBUGGER_IDLE:
    if (u16_stk_contains(&dbg->breakpoints, cpu->PC)) {
      printf("Breakpoint hit: 0x%04X\n", cpu->PC);
      dbg->state = CPU_DEBUGGER_INTERACTIVE;
      cpu->log_level = CPU_LOG_VERBOSE;
    }

    for (size_t i = 0; i < dbg->watchpoints.size; ++i) {
      const uint16_t u16 = bus_read(cpu->bus, dbg->watchpoints.data[i]);
      if (dbg->watched_memory[i] != u16) {
        printf("Watchpoint 0x%04X changed: (0x%02X -> 0x%02X)\n",
               dbg->watchpoints.data[i], dbg->watched_memory[i], u16);
        dbg->watched_memory[i] = u16;
        dbg->state = CPU_DEBUGGER_INTERACTIVE;
        cpu->log_level = CPU_LOG_VERBOSE;
      }
    }

    cpu_step(cpu);
    break;
  }
}
