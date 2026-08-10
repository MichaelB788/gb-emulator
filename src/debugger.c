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
  debugger->breakpoints = create_u16_dynamic_vec(10);
  debugger->watch_addresses = create_u16_dynamic_vec(10);
  if (debugger->breakpoints.data == NULL ||
      debugger->watch_addresses.data == NULL) {
    fprintf(stderr, "Could not create the debugger");
    return false;
  }

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

void debugger_destroy(struct debugger *debugger) {
  destroy_u16_dynamic_vec(&debugger->breakpoints);
  destroy_u16_dynamic_vec(&debugger->watch_addresses);
}

static void debugger_log_watches(const struct u16_dynamic_vec *watch_addresses,
                                 const struct bus *bus) {
  if (watch_addresses->size > 0) {
    for (size_t i = 0; i < watch_addresses->size; ++i) {
      const uint16_t addr = watch_addresses->data[i];
      printf("%04X:%02X ", addr, bus_read_byte(bus, addr));
    }
    printf("\n\n");
  }
}

static void debugger_log_r16_memory(struct debugger *debugger,
                                    const struct cpu *cpu) {
  printf("[BC]:%02X [DE]:%02X [HL]:%02X [SP]:%02X\n\n",
         bus_read_byte(cpu->bus, cpu_get_bc(cpu)),
         bus_read_byte(cpu->bus, cpu_get_de(cpu)),
         bus_read_byte(cpu->bus, cpu_get_hl(cpu)),
         bus_read_byte(cpu->bus, cpu->SP));
}

static void debugger_log_state(struct debugger *debugger,
                               const struct cpu *cpu) {
  printf("\n%s\n\n", cpu->executing_cb_op ? mnemonic_cbprefixed[cpu->IR]
                                          : mnemonic_unprefixed[cpu->IR]);
  debugger_log_watches(&debugger->watch_addresses, cpu->bus);
  debugger_log_r16_memory(debugger, cpu);
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
