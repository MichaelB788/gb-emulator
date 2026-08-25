#include "appstate.h"
#include "debugger.h"
#include "gameboy.h"
#include <errno.h>
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
  if (!create_gameboy(&app->gb, argv[1])) {
    return appstate_fail(app, "Could not create GameBoy");
  }

  // Initialize submodules based on terminal arguments
  for (size_t i = 1; i < argc; ++i) {
    if (strncmp(argv[i], "--log", 5) == 0) {
      // Logging enabled
      app->log_file = fopen("log.txt", "w");
      if (!app->log_file) {
        return appstate_fail(app, strerror(errno));
      }
    }
    if (strncmp(argv[i], "--debug", 7) == 0) {
      // Debug mode enabled
      app->debug_enabled = true;
      if (!debugger_create(&app->debugger)) {
        return appstate_fail(app, "Could not create debugger");
      }
    }
  }
  return app;
}

void appstate_iterate(struct appstate *app) {
  if (app->debug_enabled) {
    debugger_step(&app->debugger, &app->gb, app->log_file);
  } else {
    gameboy_step(&app->gb, app->log_file);
  }
}

void destroy_appstate(struct appstate *app) {
  if (app) {
    if (app->log_file) {
      fclose(app->log_file);
    }
    debugger_destroy(&app->debugger);
    destroy_gameboy(&app->gb);
    free(app);
  }
}
