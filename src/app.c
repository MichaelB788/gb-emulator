#include "app.h"
#include "gameboy.h"
#include "ppu.h"
#include "sdl_pixel_display.h"
#include <stdbool.h>
#include <stdio.h>

bool app_create(struct app *app, const char *rom, int px_scale) {
  if (!gameboy_create(&app->gameboy, rom)) {
    fprintf(stderr, "app_create: error creating gameboy\n");
    goto app_cleanup;
  }

  if (!sdl_pixel_display_create(&app->main_display, "GameBoy", SCREEN_PIXL_W,
                                SCREEN_PIXL_H, px_scale, 0)) {
    fprintf(stderr, "app_create: error creating main_display\n");
    goto app_cleanup;
  }

  return true;

app_cleanup:
  app_destroy(app);
  return false;
}

void app_destroy(struct app *app) {
  sdl_pixel_display_destroy(&app->main_display);
  gameboy_destroy(&app->gameboy);
}
