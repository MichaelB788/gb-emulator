#include "cpu_debugger.h"
#include "bus.h"
#include "cpu.h"
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
}

void cpu_debugger_destroy(struct cpu_debugger *dbg) {
  u16_stk_destroy(&dbg->breakpoints);
  u16_stk_destroy(&dbg->watches);
}

void cpu_debugger_watch_address(struct cpu_debugger *dbg) {
  push_unique_address(&dbg->watches);
}

void cpu_debugger_add_breakpoint(struct cpu_debugger *dbg) {
  push_unique_address(&dbg->breakpoints);
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
      "\n"
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

void cpu_debugger_check_for_breakpoints(struct cpu_debugger *dbg, uint16_t pc) {
  if (u16_stk_contains(&dbg->breakpoints, pc)) {
    printf("\nBreakpoint 0x%04X hit.\n", pc);
    dbg->state = CPU_DEBUGGER_BREAKPOINT_HIT;
  }
}

void cpu_debugger_init(struct cpu_debugger *dbg) {
  char user_input;
  printf("[(b)reakpoint | (w)atch | (c)ontinue]: ");
  scanf(" %c", &user_input);
  switch (user_input) {
  case 'b':
    cpu_debugger_add_breakpoint(dbg);
    break;
  case 'w':
    cpu_debugger_watch_address(dbg);
    break;
  case 'c':
    dbg->state = CPU_DEBUGGER_INACTIVE;
    break;
  default:
    break;
  }
}

void cpu_debugger_step(struct cpu_debugger *dbg, struct cpu *cpu,
                       const struct instruction *instr) {
  char user_input;
  switch (dbg->state) {
  case CPU_DEBUGGER_INACTIVE:
    break;
  case CPU_DEBUGGER_BREAKPOINT_HIT: {
    printf("[(s)tep | (b)reakpoint | (w)atch | (c)ontinue]: ");
    scanf(" %c", &user_input);
    switch (user_input) {
    case 's':
      cpu_execute_instruction(cpu, instr);
      cpu_debugger_print_cpu_step(dbg, cpu, instr);
      break;
    case 'b':
      cpu_debugger_add_breakpoint(dbg);
      break;
    case 'w':
      cpu_debugger_watch_address(dbg);
      break;
    case 'c':
      dbg->state = CPU_DEBUGGER_INACTIVE;
      break;
    default:
      break;
    }
  } break;
  case CPU_DEBUGGER_LOGGING: {
    cpu_execute_instruction(cpu, instr);
    cpu_debugger_print_cpu_step(dbg, cpu, instr);
  } break;
  }
}
