#include "debugger.h"
#include "bus.h"
#include "cpu.h"
#include "gameboy.h"
#include "mnemonics.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

bool debugger_create(struct debugger *debugger) {
  if (u16_dynamic_vec_create(&debugger->breakpoints, 10) &&
      u16_dynamic_vec_create(&debugger->watch_addresses, 10)) {
    while (true) {
      char user_input;
      unsigned value;
      printf("[(b)reakpoint | (w)atch | (c)ontinue]: ");
      scanf(" %c", &user_input);
      switch (user_input) {
      case 'b':
        printf("value: ");
        scanf("%x", &value);
        u16_dynamic_vec_push_unique(&debugger->breakpoints, value);
        break;
      case 'w':
        printf("value: ");
        scanf("%x", &value);
        u16_dynamic_vec_push_unique(&debugger->watch_addresses, value);
        break;
      case 'c':
        return true;
      default:
        break;
      }
    }
  }
  return false;
}

void debugger_destroy(struct debugger *debugger) {
  u16_dynamic_vec_destroy(&debugger->breakpoints);
  u16_dynamic_vec_destroy(&debugger->watch_addresses);
}

static void debugger_log_watches(const struct u16_dynamic_vec *watch_addresses,
                                 const struct bus *bus) {
  printf("\nwatch: ");
  for (size_t i = 0; i < watch_addresses->size; ++i) {
    const uint16_t addr = watch_addresses->data[i];
    printf("%04X:%02X ", addr, bus_read_byte(bus, addr));
  }
  printf("\n\n");
}

static void debugger_log_state(struct debugger *debugger,
                               const struct cpu *cpu) {
  debugger_log_watches(&debugger->watch_addresses, cpu->bus);

  if (cpu->executing_cb_op) {
    printf("%s\n", mnemonic_cbprefixed[cpu->IR]);
  } else {
    printf("%s\n", mnemonic_unprefixed[cpu->IR]);
  }
  cpu_log_step_reg16(cpu, stdout);
  printf("\n");
}

void debugger_check_for_breakpoints(struct debugger *debugger,
                                    const struct cpu *cpu) {
  for (size_t i = 0; i < debugger->breakpoints.size; ++i) {
    if (cpu->PC == debugger->breakpoints.data[i]) {
      debugger->debug_mode_active = true;
      printf("\nBreakpoint 0x%04X hit. Entered debug mode\n",
             debugger->breakpoints.data[i]);
      debugger_log_state(debugger, cpu);
      return;
    }
  }
}

void debugger_interactive_menu(struct debugger *debugger, struct gameboy *gb) {
  char user_input;
  unsigned value;
  printf("[(s)tep | (b)reakpoint | (w)atch | (c)ontinue]: ");
  scanf(" %c", &user_input);
  switch (user_input) {
  case 's':
    debugger_log_state(debugger, &gb->cpu);
    gameboy_step(gb);
    break;
  case 'b':
    printf("value: ");
    scanf("%x", &value);
    u16_dynamic_vec_push_unique(&debugger->breakpoints, value);
    break;
  case 'w':
    printf("value: ");
    scanf("%x", &value);
    u16_dynamic_vec_push_unique(&debugger->watch_addresses, value);
    break;
  case 'c':
    debugger->debug_mode_active = false;
    break;
  default:
    break;
  }
}
