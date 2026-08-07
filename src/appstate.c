#include "appstate.h"
#include "cpu.h"
#include "gameboy.h"
#include <stdio.h>
#include <string.h>

bool appstate_init(struct appstate *app, char **argv) {
  // Gameboy initialization
  if (!gameboy_init(&app->gb, argv[1])) {
    fprintf(stderr, "Failed to initialize the cartridge\n");
    return false;
  }

  // Log file initialization if enabled with "--log"
  if (argv[2] && strncmp(argv[2], "--log", 5) == 0) {
    app->log_file = fopen("log.txt", "w");
    if (app->log_file) {
      app->gb.cpu.log_file = app->log_file;
    } else {
      perror("Failed to create log file");
      return false;
    }
  }

  return true;
}

void appstate_update(struct appstate *app) {
  // TODO: rendering, keyboard input, etc.
  gameboy_update(&app->gb);
}

void appstate_quit(struct appstate *app) {
  if (app->log_file) {
    fclose(app->log_file);
  }
  gameboy_quit(&app->gb);
}
