#include "app.h"
#include "app_result.h"
#include "gameboy.h"
#include "graphics.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <stddef.h>
#include <stdio.h>

bool app_create(struct app *app, const char *rom, enum gb_dbg_opt opts) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
    fprintf(stderr, "app sdl_init: %s\n", SDL_GetError());
    return false;
  }

  if (!graphics_create(&app->graphics) ||
      !gameboy_create(&app->gameboy, rom, opts)) {
    app_destroy(app);
    return false;
  }

  return true;
}

void app_loop(struct app *app) {
  while (true) {
    while (SDL_PollEvent(&app->event)) {
      if (app->event.type == SDL_EVENT_QUIT)
        return;
    }

    switch (gameboy_step(&app->gameboy)) {
    case APP_CONTINUE:
      break;
    case APP_SUCCESS:
      puts("Program exiting");
      return;
    case APP_FAILURE:
      fprintf(stderr, "An error occurred\n");
      break;
    }

    graphics_render_frame(&app->graphics, &app->gameboy);

    SDL_Delay(10);
  }
}

void app_destroy(struct app *app) {
  graphics_destroy(&app->graphics);
  gameboy_destroy(&app->gameboy);
  SDL_Quit();
}
