#include "debugger.h"
#include "bus.h"
#include "cpu.h"
#include "gameboy.h"
#include "mnemonics.h"
#include "vector.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

bool debugger_has_init(struct debugger *debugger) {
  return debugger->breakpoints.data != NULL &&
         debugger->watch_addresses.data != NULL;
}

struct debugger create_debugger() {
  struct debugger debugger = {.state = DEBUG_INIT,
                              .breakpoints = create_u16_dynamic_vec(10),
                              .watch_addresses = create_u16_dynamic_vec(10)};
  if (debugger.breakpoints.data == NULL ||
      debugger.watch_addresses.data == NULL) {
    fprintf(stderr, "Failed to create debugger vectors\n");
  }
  return debugger;
}

void destroy_debugger(struct debugger *debugger) {
  destroy_u16_dynamic_vec(&debugger->breakpoints);
  destroy_u16_dynamic_vec(&debugger->watch_addresses);
}

static void debugger_log_state(struct debugger *debugger,
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
         bus_read_byte(cpu->bus, cpu_get_bc(cpu)),
         bus_read_byte(cpu->bus, cpu_get_de(cpu)),
         bus_read_byte(cpu->bus, cpu_get_hl(cpu)),
         bus_read_byte(cpu->bus, cpu->SP));

  cpu_log_step_reg16(cpu, stdout);

  printf("\n");
}

void debugger_check_for_breakpoints(struct debugger *debugger,
                                    const struct cpu *cpu) {
  for (size_t i = 0; i < debugger->breakpoints.size; ++i) {
    if (cpu->PC == debugger->breakpoints.data[i]) {
      debugger->state = DEBUG_BREAKPOINT;
      printf("\nBreakpoint 0x%04X hit. Entered debug mode\n",
             debugger->breakpoints.data[i]);
      debugger_log_state(debugger, cpu);
      return;
    }
  }
}

void debugger_initialize_variables_menu(struct debugger *debugger) {
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
    debugger->state = DEBUG_CONTINUE;
    break;
  default:
    break;
  }
}

void debugger_breakpoint_menu(struct debugger *debugger, struct gameboy *gb) {
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
    debugger->state = DEBUG_CONTINUE;
    break;
  default:
    break;
  }
}
