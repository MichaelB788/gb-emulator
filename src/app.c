#include "app.h"
#include "app_result.h"
#include "gameboy.h"
#include <SDL3/SDL_events.h>
#include <stddef.h>
#include <stdio.h>

bool app_create(struct app *app, const char *rom, enum gb_dbg_opt opts) {
  // TODO: SDL subsystem init

  // Gameboy initialization
  if (!gameboy_create(&app->gameboy, rom, opts)) {
    app_destroy(app);
    return false;
  }

  return true;
}

void app_loop(struct app *app) {
  while (true) {
    // Poll events
    while (SDL_PollEvent(&app->event)) {
      if (app->event.type == SDL_EVENT_QUIT)
        return;
    }

    // Update the GameBoy
    switch (gameboy_step(&app->gameboy)) {
    case APP_CONTINUE:
      break;
    case APP_SUCCESS:
      puts("Program exiting");
      return;
    case APP_FAILURE:
      fprintf(stderr, "An error occurred");
      break;
    }
  }
}

void app_destroy(struct app *app) { gameboy_destroy(&app->gameboy); }
