#include "appstate.h"
#include <stdio.h>
#include <stdlib.h>

struct appstate *create_app(const char *path_to_rom, bool logging_enabled) {
  struct appstate *state = malloc(sizeof(struct appstate));
  if (!state) {
    fprintf(stderr, "Malloc for appstate failed");
    return NULL;
  }

  if (!emu_init(&state->gb, path_to_rom)) {
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
    emu_close(&state->gb);

    if (state->log_file) {
      fclose(state->log_file);
    }

    free(state);
  }
}

void log_curr_instr(struct appstate *state) {
  if (state->log_file) {
  }
}
