#include "app.h"
#include "gameboy.h"
#include "pixel_display.h"
#include "ppu.h"
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

struct app {
  struct gameboy gameboy;
  struct pixel_display main_display;
  struct pixel_display vram_display;
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

#ifndef NDEBUG
  if (!pixel_display_create(&app->vram_display, "GameBoy VRAM", 128, 192,
                            px_scale, 0)) {
    fprintf(stderr, "app_create: error creating vram display\n");
    goto app_cleanup;
  }
#endif

  if (!pixel_display_create(&app->main_display, "GameBoy", SCREEN_PIXL_W,
                            SCREEN_PIXL_H, px_scale, 0)) {
    fprintf(stderr, "app_create: error creating main display\n");
    goto app_cleanup;
  }

  return app;

app_cleanup:
  app_destroy(app);
  return NULL;
}

void app_destroy(struct app *app) {
  if (app) {
    pixel_display_destroy(&app->main_display);
#ifndef NDEBUG
    pixel_display_destroy(&app->vram_display);
#endif

    gameboy_destroy(&app->gameboy);
  }
}

void app_update(struct app *app) { gameboy_step(&app->gameboy); }

void app_render(struct app *app) {
#ifndef NDEBUG
  pixel_display_render_frame(&app->vram_display);
#endif
  pixel_display_render_frame(&app->main_display);
}
