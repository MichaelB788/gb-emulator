#include "app.h"
#include "gameboy.h"
#include "sdl_pixel_display.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <stdio.h>
#include <string.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

// global app struct
struct app app = {0};

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  (void)appstate;

  if (argc < 1) {
    fprintf(stderr, "No ROM found.\n");
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_VIDEO)) {
    fprintf(stderr, "SDL_Init: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  char rom_path[FILENAME_MAX + 1];
  strncpy(rom_path, argv[1], FILENAME_MAX);

  if (!app_create(&app, rom_path, 5))
    return SDL_APP_FAILURE;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  (void)appstate;

  gameboy_step(&app.gameboy);
  sdl_pixel_display_render_frame(&app.main_display);

  SDL_Delay(10);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  (void)appstate;

  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  (void)appstate;

  app_destroy(&app);

  switch (result) {
  case SDL_APP_CONTINUE:
    break;
  case SDL_APP_SUCCESS:
    puts("Program success");
    break;
  case SDL_APP_FAILURE:
    puts("Program failure");
    break;
  }
}
