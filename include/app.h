#pragma once
#include "gameboy.h"
#include <SDL3/SDL_events.h>

struct app {
  struct gameboy gameboy;
  SDL_Event event;
};

[[nodiscard]] struct app *app_malloc(const char *rom,
                                     enum gb_debug_option opts);
void app_free(struct app *state);

void app_loop(struct app *app);
