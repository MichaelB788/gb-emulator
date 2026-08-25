#include "appstate.h"
#include "debugger.h"
#include "gameboy.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct appstate *appstate_malloc(int argc, char **argv) {
  struct appstate *app = malloc(sizeof(struct appstate));

  // Gameboy initialization
  if (!gameboy_create(&app->gameboy, argv[1])) {
    free(app);
    return nullptr;
  }

  // Initialize submodules based on terminal arguments
  for (size_t i = 1; i < argc; ++i) {
    // Logging enabled
    if (strncmp(argv[i], "--log", 5) == 0) {
      app->log_file = fopen("log.txt", "w");
      if (app->log_file == nullptr) {
        perror("appstate_malloc");
        appstate_free(app);
        return nullptr;
      }
    }

    // Debug mode enabled
    if (strncmp(argv[i], "--debug", 7) == 0) {
      app->debug_enabled = true;
      debugger_create(&app->debugger);
    }
  }
  return app;
}

void appstate_iterate(struct appstate *app) {
  if (app->debug_enabled)
    debugger_step(&app->debugger, &app->gameboy, app->log_file);
  else
    gameboy_step(&app->gameboy, app->log_file);
}

void appstate_free(struct appstate *app) {
  if (app) {
    if (app->log_file)
      fclose(app->log_file);
    debugger_destroy(&app->debugger);
    gameboy_destroy(&app->gameboy);
    free(app);
  }
}
