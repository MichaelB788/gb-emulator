#include "cpu_debugger.h"
#include "cpu.h"
#include "instruction.h"
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

// Pushes a u16 address to `out` via terminal input
static void push_unique_address_from_input(struct u16_stk *out) {
  unsigned address;
  printf("address: ");
  scanf("%x", &address);
  if (!u16_stk_contains(out, address))
    u16_stk_push(out, address);
}

void cpu_debugger_create(struct cpu_debugger *dbg) {
  u16_stk_create(&dbg->breakpoints, 10);
}

void cpu_debugger_destroy(struct cpu_debugger *dbg) {
  u16_stk_destroy(&dbg->breakpoints);
}

void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu) {
  switch (dbg->state) {
  case CPU_DEBUGGER_INIT: {
    char user_input;
    printf("[(b)reakpoint | (c)ontinue]: ");
    scanf(" %c", &user_input);
    switch (user_input) {
    case 'b':
      push_unique_address_from_input(&dbg->breakpoints);
      break;
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
      printf("\nBreakpoint 0x%04X hit.\n", cpu->PC);
      dbg->state = CPU_DEBUGGER_BREAKPOINT_HIT;
      cpu->log_level = CPU_LOG_VERBOSE;
    }
    cpu_step(cpu);
  } break;
  case CPU_DEBUGGER_BREAKPOINT_HIT: {
    char user_input;
    printf("[(s)tep | (b)reakpoint | (c)ontinue]: ");
    scanf(" %c", &user_input);
    switch (user_input) {
    case 's':
      cpu_step(cpu);
      break;
    case 'b':
      push_unique_address_from_input(&dbg->breakpoints);
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
