#include "appstate.h"
#include "optables.h"
#include <stdio.h>
#include <stdlib.h>

struct appstate *create_app(const char *path_to_rom, bool logging_enabled) {
  struct appstate *state = malloc(sizeof(struct appstate));
  if (!state) {
    fprintf(stderr, "Malloc for appstate failed");
    return NULL;
  }

  if (!init_gameboy(&state->gb, path_to_rom)) {
    free(state);
    return NULL;
  }

  if (logging_enabled) {
    state->log_file = fopen("log.txt", "w");
    if (!state->log_file) {
      perror("Could not open log file");
    }
  }

  return state;
}

void destroy_app(struct appstate *state) {
  if (state) {
    close_gameboy(&state->gb);

    if (state->log_file) {
      fclose(state->log_file);
    }

    free(state);
  }
}

void log_curr_instr(struct appstate *state) {
  if (state->log_file) {
    const struct cpu *cpu = &state->gb.cpu;
    fprintf(state->log_file,
            "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X "
            "PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
            cpu->A, cpu->F, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L,
            cpu->SP, cpu->PC, bus_read(&state->gb, cpu->PC),
            bus_read(&state->gb, cpu->PC + 1),
            bus_read(&state->gb, cpu->PC + 2),
            bus_read(&state->gb, cpu->PC + 3));
    fflush(state->log_file);
  }
}

int cpu_step(struct appstate *state) {
  if (state->gb.cpu.halt_mode) {
    return 4;
  }

  if (state->gb.cpu.enable_interrupts) {
    state->gb.cpu.IME = true;
    state->gb.cpu.enable_interrupts = false;
  }

  state->gb.opcode = bus_read(&state->gb, state->gb.cpu.PC);
  log_curr_instr(state);
  ++state->gb.cpu.PC;

  return unprefixed_ins[state->gb.opcode](&state->gb);
}
