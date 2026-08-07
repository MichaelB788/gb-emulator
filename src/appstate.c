#include "appstate.h"
#include "cpu.h"
#include "debugger.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

bool appstate_init(struct appstate *app, char **argv) {
  // Gameboy initialization
  if (!gameboy_init(&app->gb, argv[1])) {
    fprintf(stderr, "Failed to initialize the cartridge\n");
    return false;
  }

  if (argv[2] && strncmp(argv[2], "--log", 5) == 0) {
    // Log file initialization
    app->log_file = fopen("log.txt", "w");
    if (app->log_file) {
      app->gb.cpu.log_file = app->log_file;
    } else {
      perror("Failed to create log file");
      return false;
    }
  } else if (argv[2] && strncmp(argv[2], "--debug", 7) == 0) {
    // Debug mode initialization
    app->debug_enabled = true;
    debugger_create(&app->debugger);
  }

  return true;
}

void appstate_update(struct appstate *app) {
  if (app->debug_enabled) {
    if (app->debugger.debug_mode_active) {
      debugger_interactive_menu(&app->debugger, &app->gb);
    } else {
      debugger_check_for_breakpoints(&app->debugger, &app->gb.cpu);
      gameboy_step(&app->gb);
    }
  } else {
    gameboy_step(&app->gb);
  }
}

void appstate_quit(struct appstate *app) {
  if (app->log_file) {
    fclose(app->log_file);
  }
  debugger_destroy(&app->debugger);
  gameboy_quit(&app->gb);
}
