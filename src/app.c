#include "app.h"
#include "gameboy.h"
#include <SDL3/SDL_events.h>
#include <stddef.h>
#include <stdlib.h>

struct app *app_malloc(const char *rom_path, bool debug_enabled) {
  struct app *app = malloc(sizeof(struct app));

  // Gameboy initialization
  if (!gameboy_create(&app->gameboy, rom_path)) {
    free(app);
    return nullptr;
  }
  if (debug_enabled)
    gameboy_enable_debug(&app->gameboy);

  return app;
}

void app_loop(struct app *app) {
  while (true) {
    // Poll events
    while (SDL_PollEvent(&app->event)) {
      if (app->event.type == SDL_EVENT_QUIT)
        return;
    }

    // Update the GameBoy
    gameboy_step(&app->gameboy);
  }
}

void app_free(struct app *app) {
  if (app) {
    gameboy_destroy(&app->gameboy);
    free(app);
  }
}
