#include "debugger.h"
#include "bus.h"
#include "cpu.h"
#include "gameboy.h"
#include "mnemonics.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void u16_stk_create(struct u16_stk *stk) {
  stk->size = 0;
  stk->data = malloc(stk->capacity = 10);
}

static void u16_stk_destroy(struct u16_stk *stk) {
  stk->size = stk->capacity = 0;
  if (stk->data)
    free(stk->data);
}

static bool u16_stk_contains(const struct u16_stk *stk, uint16_t u16) {
  for (size_t i = 0; i < stk->size; ++i) {
    if (stk->data[i] == u16)
      return true;
  }
  return false;
}

static bool u16_stk_push(struct u16_stk *stk, uint16_t u16) {
  if (stk->size == stk->capacity) {
    stk->capacity *= 2;
    stk->data = realloc(stk->data, stk->capacity * 2);
  }

  stk->data[stk->size++] = u16;
  return true;
}

void debugger_create(struct debugger *debugger) {
  u16_stk_create(&debugger->breakpoints);
  u16_stk_create(&debugger->watch_addresses);
  debugger->state = DEBUG_INIT;
}

void debugger_destroy(struct debugger *debugger) {
  u16_stk_destroy(&debugger->breakpoints);
  u16_stk_destroy(&debugger->watch_addresses);
}

// Pushes a u16 address to `out` via terminal input
static void add_unique_address(struct u16_stk *out) {
  unsigned address;
  printf("address: ");
  scanf("%x", &address);
  if (!u16_stk_contains(out, address)) {
    u16_stk_push(out, address);
  }
}

// Prints the current CPU state to `stdout`
static void debugger_print_cpu_step(struct debugger *debugger,
                                    const struct cpu *cpu) {
  // Current opcode name
  printf("\n%s\n\n", cpu->executing_cb_op ? mnemonic_cbprefixed[cpu->IR]
                                          : mnemonic_unprefixed[cpu->IR]);

  // Watch addresses
  if (debugger->watch_addresses.size > 0) {
    for (size_t i = 0; i < debugger->watch_addresses.size; ++i) {
      const uint16_t addr = debugger->watch_addresses.data[i];
      printf("%04X:%02X ", addr, bus_read_byte(cpu->bus, addr));
    }
    printf("\n\n");
  }

  // R16 memory watches
  printf("[BC]:%02X [DE]:%02X [HL]:%02X [SP]:%02X\n\n",
         bus_read_byte(cpu->bus, cpu->BC), bus_read_byte(cpu->bus, cpu->DE),
         bus_read_byte(cpu->bus, cpu->HL), bus_read_byte(cpu->bus, cpu->SP));

  cpu_log_state_reg16(cpu, stdout);

  printf("\n");
}

void debugger_step(struct debugger *debugger, struct gameboy *gb,
                   FILE *log_file) {
  switch (debugger->state) {
  case DEBUG_INIT: {
    printf("[(b)reakpoint | (w)atch | (c)ontinue]: ");
    char user_input;
    scanf(" %c", &user_input);
    switch (user_input) {
    case 'b':
      add_unique_address(&debugger->breakpoints);
      break;
    case 'w':
      add_unique_address(&debugger->watch_addresses);
      break;
    case 'c':
      debugger->state = DEBUG_CONTINUE;
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
      debugger_print_cpu_step(debugger, &gb->cpu);
      gameboy_step(gb, log_file);
      break;
    case 'b':
      add_unique_address(&debugger->breakpoints);
      break;
    case 'w':
      add_unique_address(&debugger->watch_addresses);
      break;
    case 'c':
      debugger->state = DEBUG_CONTINUE;
      break;
    default:
      break;
    }
  } break;
  case DEBUG_CONTINUE: {
    if (u16_stk_contains(&debugger->breakpoints, gb->cpu.PC)) {
      printf("\nBreakpoint 0x%04X hit.\n\n", gb->cpu.PC);
      debugger->state = DEBUG_BREAKPOINT;
    } else {
      gameboy_step(gb, log_file);
    }
  } break;
  }
}
