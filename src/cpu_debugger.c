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
static void push_unique_address_from_input(struct u16_stk *out) {
  uint16_t address;
  printf("Address: ");
  scanf("%hu", &address);
  if (!u16_stk_contains(out, address)) {
    if (!u16_stk_push(out, address))
      fprintf(stderr, "Stack at max capacity. Value discarded.\n");
  }
}

static void cpu_debugger_print_watch_addresses(const struct cpu_debugger *dbg,
                                               const struct bus *bus) {
  static constexpr size_t WATCH_ENTRY_SIZE = sizeof "[0000]:00 ";

  if (dbg->watchpoints.size > 0) {
    char buffer[dbg->watchpoints.size * WATCH_ENTRY_SIZE];
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
  DBG_STEP       = 1 << 2,
  // clang-format on
};
static void cpu_debugger_parse_input(struct cpu_debugger *dbg, struct cpu *cpu,
                                     char input, enum dbg_flags flags) {
  switch (input) {
  case 's':
    if (flags & DBG_STEP) {
      cpu_step(cpu);
      return;
    }
    break;
  case 'b':
    if (flags & DBG_BREAKPOINT) {
      push_unique_address_from_input(&dbg->breakpoints);
      return;
    }
    break;
  case 'w':
    if (flags & DBG_WATCHPOINT) {
      push_unique_address_from_input(&dbg->watchpoints);
      return;
    }
    break;
  case 'c':
    dbg->state = CPU_DEBUGGER_IDLE;
    cpu->log_level = CPU_LOG_NONE;
    return;
  }
  puts("Invalid option.");
}

void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu) {
  switch (dbg->state) {
  case CPU_DEBUGGER_INIT:
    char user_input;
    printf("\n[(b)reakpoint | (w)atchpoint | (c)ontinue]: ");
    scanf(" %c", &user_input);
    putchar('\n');
    cpu_debugger_parse_input(dbg, cpu, user_input,
                             DBG_BREAKPOINT | DBG_WATCHPOINT);
    break;
  case CPU_DEBUGGER_INTERACTIVE: {
    char user_input;
    printf("\n[(s)tep | (b)reakpoint | (w)atchpoint | (c)ontinue]: ");
    scanf(" %c", &user_input);
    putchar('\n');
    cpu_debugger_parse_input(dbg, cpu, user_input,
                             DBG_STEP | DBG_BREAKPOINT | DBG_WATCHPOINT);
  } break;
  case CPU_DEBUGGER_IDLE: {
    if (u16_stk_contains(&dbg->breakpoints, cpu->PC)) {
      printf("Breakpoint 0x%04X hit.\n", cpu->PC);
      dbg->state = CPU_DEBUGGER_INTERACTIVE;
      cpu->log_level = CPU_LOG_VERBOSE;
    }
    cpu_step(cpu);
  } break;
  }
}
