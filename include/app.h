#pragma once
#include "gameboy.h"
#include <SDL3/SDL_events.h>

struct app {
  struct gameboy gameboy;
  SDL_Event event;
};

[[nodiscard]] bool app_create(struct app *app, const char *rom,
                              enum gb_dbg_opt opts);
void app_destroy(struct app *state);

void app_loop(struct app *app);
