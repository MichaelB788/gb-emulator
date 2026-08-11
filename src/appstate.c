#include "appstate.h"
#include "cpu.h"
#include "debugger.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct appstate *create_appstate(int argc, char **argv) {
  struct appstate *app = malloc(sizeof(struct appstate));

  // Gameboy initialization
  if (!gameboy_init(&app->gb, argv[1])) {
    fprintf(stderr, "Failed to create the GameBoy\n");
    goto appstate_fail;
  }

  // Initialize submodules based on terminal arguments
  for (size_t i = 1; i < argc; ++i) {
    if (strncmp(argv[i], "--log", 5) == 0) {
      // Log file initialization
      app->log_file = fopen("log.txt", "w");
      if (app->log_file) {
        app->gb.cpu.log_file = app->log_file;
        continue;
      } else {
        perror("Failed to create log file");
        goto appstate_fail;
      }
    }
    if (strncmp(argv[i], "--debug", 7) == 0) {
      // Debug mode initialization
      app->debug_enabled = true;
      app->debugger = create_debugger();
      if (debugger_has_init(&app->debugger)) {
        continue;
      } else {
        fprintf(stderr, "Failed to create the debugger\n");
        goto appstate_fail;
      }
    }
  }

  return app;

appstate_fail:
  free(app);
  return NULL;
}

void appstate_iterate(struct appstate *app) {
  if (app->debug_enabled) {
    switch (app->debugger.state) {
    case DEBUG_INIT:
      debugger_initialize_variables_menu(&app->debugger);
      break;
    case DEBUG_BREAKPOINT:
      debugger_breakpoint_menu(&app->debugger, &app->gb);
      break;
    case DEBUG_CONTINUE:
      debugger_check_for_breakpoints(&app->debugger, &app->gb.cpu);
      gameboy_step(&app->gb);
      break;
    }
  } else {
    gameboy_step(&app->gb);
  }
}

void destroy_appstate(struct appstate *app) {
  if (app) {
    if (app->log_file) {
      fclose(app->log_file);
    }
    destroy_debugger(&app->debugger);
    gameboy_quit(&app->gb);
    free(app);
  }
}
