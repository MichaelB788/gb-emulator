#include "debugger.h"
#include "cpu.h"
#include "dynamic_word_mem.h"
#include <stdio.h>

bool debugger_init(struct debugger *debugger) {
  bool success = dynamic_word_mem_create(&debugger->breakpoints, 10) &&
                 dynamic_word_mem_create(&debugger->watch_addresses, 10);
  if (!success) {
    return false;
  }

  // TODO: Initialize breakpoints and watch addresses through terminal input

  return true;
}

bool debugger_add_breakpoint(struct debugger *debugger, uint16_t addr) {
  return dynamic_word_mem_push(&debugger->breakpoints, addr);
}

bool debugger_add_watch_address(struct debugger *debugger, uint16_t addr) {
  return dynamic_word_mem_push(&debugger->watch_addresses, addr);
}

void debugger_step(struct debugger *debugger, struct cpu *cpu) {
  char user_input;
  scanf("%c", &user_input);
  switch (user_input) {
  case 'n':
    cpu_log_current_step(cpu, stdout);
    cpu_step(cpu);
    break;
  default:
    break;
  }
}

void debugger_destroy(struct debugger *debugger) {
  dynamic_word_mem_destroy(&debugger->breakpoints);
  dynamic_word_mem_destroy(&debugger->watch_addresses);
}
