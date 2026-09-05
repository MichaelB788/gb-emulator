#include "app.h"
#include "cpu.h"
#include "gameboy.h"
#include <SDL3/SDL_events.h>
#include <stddef.h>
#include <stdlib.h>

struct app *app_malloc(const char *rom_path, bool cpu_debugging_enabled) {
  struct app *app = malloc(sizeof(struct app));

  // Gameboy initialization
  if (!gameboy_create(&app->gameboy, rom_path)) {
    free(app);
    return nullptr;
  }

  // Enable debugging
  if (cpu_debugging_enabled)
    cpu_enable_debugging(&app->gameboy.cpu);

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
