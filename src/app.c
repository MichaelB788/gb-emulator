#include "app.h"
#include "gameboy.h"
#include "pixel_display.h"
#include "ppu.h"
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// the GameBoy runs at 59.7 FPS (slightly slower than 60 FPS)
#define TARGET_FPS_NS ((uint64_t)(SDL_NS_PER_SECOND / 59.7))

struct app {
  uint64_t prev_time, curr_time;
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

// not a very accurate way of printing fps, since it logs integer units, but it
// kind of works
static void print_fps(uint64_t delta_time) {
  static int fps_count = 0;
  static uint64_t fps_accumulator = 0;

  fps_count++;
  fps_accumulator += delta_time;

  if (fps_accumulator >= SDL_SECONDS_TO_NS(1)) {
    printf("%d\n", fps_count);
    fps_accumulator -= SDL_SECONDS_TO_NS(1);
    fps_count = 0;
  }
}

void app_update(struct app *app) {
  app->prev_time = app->curr_time;
  app->curr_time = SDL_GetTicksNS();
  const uint16_t delta_time = app->curr_time - app->prev_time;

  gameboy_step(&app->gameboy);
}

void app_render(struct app *app) {
#ifndef NDEBUG
  pixel_display_draw_static_test(&app->vram_display);
  pixel_display_render_frame(&app->vram_display);
#endif
  pixel_display_draw_static_test(&app->main_display);
  pixel_display_render_frame(&app->main_display);
}

void app_sleep(struct app *app) {
  const uint64_t elapsed = SDL_GetTicksNS() - app->curr_time;
  if (elapsed < TARGET_FPS_NS)
    SDL_DelayNS(TARGET_FPS_NS - elapsed);
}
