#include "appstate.h"
#include "cpu.h"
#include "debugger.h"
#include "gameboy.h"
#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void *appstate_fail(struct appstate *app, char *msg) {
  fprintf(stderr, "Failed to create appstate: %s\n", msg);
  destroy_appstate(app);
  return NULL;
}

struct appstate *create_appstate(int argc, char **argv) {
  struct appstate *app = malloc(sizeof(struct appstate));

  // Gameboy initialization
  if (!gameboy_init(&app->gb, argv[1])) {
    return appstate_fail(app, "Could not create GameBoy");
  }

  // Disable debug, just in case
  app->debugger.state = DEBUG_DISABLED;

  // Initialize submodules based on terminal arguments
  for (size_t i = 1; i < argc; ++i) {
    if (strncmp(argv[i], "--log", 5) == 0) {
      // Logging enabled
      app->log_file = fopen("log.txt", "w");
      if (!app->log_file) {
        return appstate_fail(app, strerror(errno));
      }
      app->gb.cpu.log_file = app->log_file;
    }
    if (strncmp(argv[i], "--debug", 7) == 0) {
      // Debug mode enabled
      app->debug_enabled = true;
      if (!create_debugger(&app->debugger)) {
        return appstate_fail(app, "Could not create debugger");
      }
    }
  }
  return app;
}

void appstate_iterate(struct appstate *app) {
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
  case DEBUG_DISABLED:
    gameboy_step(&app->gb);
    break;
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
