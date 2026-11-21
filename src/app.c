#include "app.h"
#include "dmgb/dmgb.h"
#include "dmgb/ppu/ppu.h"
#include "pixel_display.h"
#include <SDL3/SDL_timer.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// the GameBoy runs at 59.7 FPS (slightly slower than 60 FPS)
#define TARGET_FPS_NS ((uint64_t)(SDL_NS_PER_SECOND / 59.7))

static void app_print_fps(uint64_t delta_time);

struct app {
  uint64_t prev_time, curr_time;
  struct dmgb dmgb;
  struct pixel_display main_display;
  struct pixel_display vram_tile_data_display;
};

struct app *app_create(const char *rom, int px_scale) {
  struct app *app = NULL;
  if ((app = calloc(1, sizeof(struct app))) == NULL) {
    perror("app_create calloc");
    goto app_cleanup;
  }

  if (!dmgb_create(&app->dmgb, rom)) {
    fprintf(stderr, "app_create: error creating gameboy\n");
    goto app_cleanup;
  }

#ifndef NDEBUG
  if (!pixel_display_create(&app->vram_tile_data_display,
                            "GameBoy VRAM Tile Data", VRAM_DATA_SCREEN_PIXEL_W,
                            VRAM_DATA_SCREEN_PIXEL_H, px_scale, 0)) {
    fprintf(stderr, "app_create: error creating vram display\n");
    goto app_cleanup;
  }
#endif
  if (!pixel_display_create(&app->main_display, "GameBoy", SCREEN_PIXEL_W,
                            SCREEN_PIXEL_H, px_scale, 0)) {
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
    pixel_display_destroy(&app->vram_tile_data_display);
#endif

    dmgb_destroy(&app->dmgb);
  }
}

void app_update(struct app *app) {
  app->prev_time = app->curr_time;
  app->curr_time = SDL_GetTicksNS();
  // const uint16_t delta_time = app->curr_time - app->prev_time;

  dmgb_step(&app->dmgb);
}

void app_render(struct app *app) {
#ifndef NDEBUG
  ppu_draw_vram_tile_data(&app->dmgb.ppu, &app->vram_tile_data_display);
#endif
  pixel_display_draw_static_test(&app->main_display);

#ifndef NDEBUG
  pixel_display_present_frame(&app->vram_tile_data_display);
#endif
  pixel_display_present_frame(&app->main_display);
}

void app_sleep(const struct app *app) {
  const uint64_t elapsed = SDL_GetTicksNS() - app->curr_time;
  if (elapsed < TARGET_FPS_NS)
    SDL_DelayNS(TARGET_FPS_NS - elapsed);
}

void app_print_fps(uint64_t delta_time) {
  // not a very accurate way of printing fps, since it logs integer units, but
  // it kind of works
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
