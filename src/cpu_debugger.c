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
  unsigned address;
  printf("Address: ");
  scanf("%x", &address);
  if (!u16_stk_contains(out, address)) {
    if (!u16_stk_push(out, address))
      fprintf(stderr, "Stack at max capacity. Value discarded.\n");
  }
}

static void cpu_debugger_print_watch_addresses(const struct cpu_debugger *dbg,
                                               const struct bus *bus) {
  static constexpr size_t WATCH_ENTRY_SIZE = sizeof "[0000]:00 ";

  if (dbg->watch_addresses.size > 0) {
    char buffer[dbg->watch_addresses.size * WATCH_ENTRY_SIZE];
    for (size_t i = 0; i < dbg->watch_addresses.size; ++i) {
      const uint16_t addr = dbg->watch_addresses.data[i];
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
  dbg->breakpoints.size = dbg->watch_addresses.size = 0;
}

void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu) {
  switch (dbg->state) {
  case CPU_DEBUGGER_INIT: {
    char user_input;
    printf("[(b)reakpoint | (w)atch | (c)ontinue]: ");
    scanf(" %c", &user_input);
    switch (user_input) {
    case 'b':
      push_unique_address_from_input(&dbg->breakpoints);
      break;
    case 'w':
      push_unique_address_from_input(&dbg->watch_addresses);
      break;
    case 'c':
      dbg->state = CPU_DEBUGGER_WATCH_FOR_BREAKPOINTS;
      break;
    default:
      break;
    }
  } break;
  case CPU_DEBUGGER_WATCH_FOR_BREAKPOINTS: {
    if (u16_stk_contains(&dbg->breakpoints, cpu->PC)) {
      printf("\n"
             "Breakpoint 0x%04X hit.\n"
             "\n",
             cpu->PC);
      dbg->state = CPU_DEBUGGER_BREAKPOINT_HIT;
      cpu->log_level = CPU_LOG_VERBOSE;
    }
    cpu_step(cpu);
  } break;
  case CPU_DEBUGGER_BREAKPOINT_HIT: {
    char user_input;
    printf("[(s)tep | (b)reakpoint | (w)atch | (c)ontinue]: ");
    scanf(" %c", &user_input);
    putchar('\n');
    switch (user_input) {
    case 's':
      cpu_step(cpu);
      break;
    case 'b':
      push_unique_address_from_input(&dbg->breakpoints);
      break;
    case 'w':
      push_unique_address_from_input(&dbg->watch_addresses);
      break;
    case 'c':
      dbg->state = CPU_DEBUGGER_WATCH_FOR_BREAKPOINTS;
      cpu->log_level = CPU_LOG_NONE;
      break;
    default:
      break;
    }
  } break;
  }
}
