#include "app.h"
#include "app_result.h"
#include "gameboy.h"
#include "ppu.h"
#include "sdl_pixel_display.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <stdbool.h>
#include <stdio.h>

bool app_create(struct app *app, const char *rom, enum gb_dbg_opt opts) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    fprintf(stderr, "app sdl_init: %s\n", SDL_GetError());
    goto app_cleanup;
  }

  if (!sdl_pixel_display_create(&app->main_display, "GameBoy", SCREEN_PIXL_W,
                                SCREEN_PIXL_H, 5, 0) ||
      !gameboy_create(&app->gameboy, rom, opts)) {
    goto app_cleanup;
  }

  return true;

app_cleanup:
  app_destroy(app);
  return false;
}

void app_loop(struct app *app) {
  while (1) {
    while (SDL_PollEvent(&app->event)) {
      if (app->event.type == SDL_EVENT_QUIT)
        return;
    }

    switch (gameboy_step(&app->gameboy)) {
    case APP_CONTINUE:
      break;
    case APP_SUCCESS:
    case APP_FAILURE:
      return;
    }

    sdl_pixel_display_render_frame(&app->main_display);

    SDL_Delay(10);
  }
}

void app_destroy(struct app *app) {
  sdl_pixel_display_destroy(&app->main_display);

  gameboy_destroy(&app->gameboy);
  SDL_Quit();
}
