#include "app.h"
#include "gameboy.h"
#include "ppu.h"
#include "sdl_pixel_display.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct app {
  struct gameboy gameboy;
  struct sdl_pixel_display main_display;
};

struct app *app_create(const char *rom, int px_scale) {
  struct app *app = NULL;
  if ((app = calloc(1, sizeof(struct app))) == NULL) {
    perror("app_create calloc");
    goto app_cleanup;
  }

  if (!gameboy_create(&app->gameboy, rom)) {
    fprintf(stderr, "app_create: error creating gameboy\n");
    goto app_cleanup;
  }

  if (!sdl_pixel_display_create(&app->main_display, "GameBoy", SCREEN_PIXL_W,
                                SCREEN_PIXL_H, px_scale, 0)) {
    fprintf(stderr, "app_create: error creating main_display\n");
    goto app_cleanup;
  }

  return app;

app_cleanup:
  app_destroy(app);
  return NULL;
}

void app_destroy(struct app *app) {
  sdl_pixel_display_destroy(&app->main_display);
  gameboy_destroy(&app->gameboy);
}

void app_update(struct app *app) { gameboy_step(&app->gameboy); }

void app_render(struct app *app) {
  sdl_pixel_display_render_frame(&app->main_display);
}
