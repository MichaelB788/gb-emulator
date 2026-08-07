#include "debugger.h"
#include "cpu.h"
#include "vector.h"
#include <stddef.h>
#include <stdio.h>

bool debugger_init(struct debugger *debugger) {
  return u16_dynamic_vec_create(&debugger->breakpoints, 10) &&
         u16_dynamic_vec_create(&debugger->watch_addresses, 10);
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

void debugger_close(struct debugger *debugger) {
  u16_dynamic_vec_destroy(&debugger->breakpoints);
  u16_dynamic_vec_destroy(&debugger->watch_addresses);
}
