#include "cpu_debugger.h"
#include "bus.h"
#include "cpu.h"
#include "gameboy.h"
#include "instruction.h"
#include "u16_stk.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Pushes a u16 address to `out` via terminal input
static void push_unique_address(struct u16_stk *out) {
  unsigned address;
  printf("address: ");
  scanf("%x", &address);
  if (!u16_stk_contains(out, address))
    u16_stk_push(out, address);
}

void cpu_debugger_create(struct cpu_debugger *dbg) {
  u16_stk_create(&dbg->breakpoints, 10);
  u16_stk_create(&dbg->watches, 10);
  dbg->state = DEBUG_INIT;
}

void cpu_debugger_destroy(struct cpu_debugger *dbg) {
  u16_stk_destroy(&dbg->breakpoints);
  u16_stk_destroy(&dbg->watches);
}

void cpu_debugger_print_cpu_step(const struct cpu_debugger *dbg,
                                 const struct cpu *cpu,
                                 const struct instruction *instr) {
  // Create the string containing the watch addresses
  char watches_string[50] = "";
  if (dbg->watches.size > 0) {
    char *str_tgt = watches_string;
    for (size_t i = 0; i < dbg->watches.size; ++i) {
      const uint16_t addr = dbg->watches.data[i];
      str_tgt +=
          sprintf(str_tgt, "%04X:%02X ", addr, bus_read_byte(cpu->bus, addr));
    }
  } else {
    strcat(watches_string, "No watch");
  }

  printf(
      "%s" /* Instruction mnemonic */
      "\n"
      "\n"
      "%s" /* Watch addresses */
      "\n"
      "\n"
      "[BC]:%02X [DE]:%02X [HL]:%02X [SP]:%02X"
      "\n"
      "\n"
      "AF:%04X BC:%04X DE:%04X HL:%04X SP:%04X PC:%04X PCMEM:%02X,%02X,%02X,%02X"
      "\n"
      "\n",
      instr->mnemonic, watches_string, bus_read_byte(cpu->bus, cpu->BC),
      bus_read_byte(cpu->bus, cpu->DE), bus_read_byte(cpu->bus, cpu->HL),
      bus_read_byte(cpu->bus, cpu->SP), cpu->AF, cpu->BC, cpu->DE, cpu->HL,
      cpu->SP, cpu->PC, bus_read_byte(cpu->bus, cpu->PC),
      bus_read_byte(cpu->bus, cpu->PC + 1),
      bus_read_byte(cpu->bus, cpu->PC + 2),
      bus_read_byte(cpu->bus, cpu->PC + 3));
}

// TODO
void cpu_debugger_step(struct cpu_debugger *dbg, struct gameboy *gb) {
  switch (dbg->state) {
  case DEBUG_INIT: {
    printf("[(b)reakpoint | (w)atch | (c)ontinue]: ");
    char user_input;
    scanf(" %c", &user_input);
    switch (user_input) {
    case 'b':
      push_unique_address(&dbg->breakpoints);
      break;
    case 'w':
      push_unique_address(&dbg->watches);
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
      // TODO: Log step here
      gameboy_step(gb);
      break;
    case 'b':
      push_unique_address(&dbg->breakpoints);
      break;
    case 'w':
      push_unique_address(&dbg->watches);
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
      gameboy_step(gb);
    }
  } break;
  }
}
