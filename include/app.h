#pragma once
#include "gameboy.h"
#include "sdl_pixel_display.h"
#include <SDL3/SDL_events.h>
#include <stdbool.h>

struct app {
  struct gameboy gameboy;
  struct sdl_pixel_display main_display;
};

bool app_create(struct app *app, const char *rom, int px_scale);
void app_destroy(struct app *state);
