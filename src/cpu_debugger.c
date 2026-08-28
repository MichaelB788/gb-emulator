#include "cpu_debugger.h"
#include "bus.h"
#include "cpu.h"
#include "gameboy.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void cpu_debugger_create(struct cpu_debugger *dbg) {
  u16_stk_create(&dbg->breakpoints, 10);
  u16_stk_create(&dbg->watch_addresses, 10);
  dbg->state = DEBUG_INIT;
}

void cpu_debugger_destroy(struct cpu_debugger *dbg) {
  u16_stk_destroy(&dbg->breakpoints);
  u16_stk_destroy(&dbg->watch_addresses);
}

// Pushes a u16 address to `out` via terminal input
static void add_unique_address(struct u16_stk *out) {
  unsigned address;
  printf("address: ");
  scanf("%x", &address);
  if (!u16_stk_contains(out, address))
    u16_stk_push(out, address);
}

// Prints the current CPU state to `stdout`
static void dbg_print_cpu_step(struct cpu_debugger *dbg,
                               const struct cpu *cpu) {
  if (dbg->watch_addresses.size > 0) {
    for (size_t i = 0; i < dbg->watch_addresses.size; ++i) {
      const uint16_t addr = dbg->watch_addresses.data[i];
      printf("%04X:%02X ", addr, bus_read_byte(cpu->bus, addr));
    }
    printf("\n\n");
  }

  printf("[BC]:%02X [DE]:%02X [HL]:%02X [SP]:%02X\n\n",
         bus_read_byte(cpu->bus, cpu->BC), bus_read_byte(cpu->bus, cpu->DE),
         bus_read_byte(cpu->bus, cpu->HL), bus_read_byte(cpu->bus, cpu->SP));

  cpu_log_state_reg16(cpu, stdout);

  printf("\n");
}

void cpu_debugger_step(struct cpu_debugger *dbg, struct gameboy *gb,
                       FILE *log_file) {
  switch (dbg->state) {
  case DEBUG_INIT: {
    printf("[(b)reakpoint | (w)atch | (c)ontinue]: ");
    char user_input;
    scanf(" %c", &user_input);
    switch (user_input) {
    case 'b':
      add_unique_address(&dbg->breakpoints);
      break;
    case 'w':
      add_unique_address(&dbg->watch_addresses);
      break;
    case 'c':
      dbg->state = DEBUG_CONTINUE;
      break;
    default:
      break;
    }
  } break;
  case DEBUG_BREAKPOINT: {
    printf("[(s)tep | (b)reakpoint | (w)atch | (c)ontinue]: ");
    char user_input;
    scanf(" %c", &user_input);
    switch (user_input) {
    case 's':
      dbg_print_cpu_step(dbg, &gb->cpu);
      gameboy_step(gb, log_file);
      break;
    case 'b':
      add_unique_address(&dbg->breakpoints);
      break;
    case 'w':
      add_unique_address(&dbg->watch_addresses);
      break;
    case 'c':
      dbg->state = DEBUG_CONTINUE;
      break;
    default:
      break;
    }
  } break;
  case DEBUG_CONTINUE: {
    if (u16_stk_contains(&dbg->breakpoints, gb->cpu.PC)) {
      printf("\nBreakpoint 0x%04X hit.\n\n", gb->cpu.PC);
      dbg->state = DEBUG_BREAKPOINT;
    } else {
      gameboy_step(gb, log_file);
    }
  } break;
  }
}
