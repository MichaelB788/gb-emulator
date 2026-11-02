#include "app.h"
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_timer.h>
#include <stdio.h>
#include <string.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
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

  if ((*appstate = app_create(rom_path, 5)) == NULL)
    return SDL_APP_FAILURE;

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  app_update(appstate);
  app_render(appstate);

  SDL_Delay(10); // todo 59.7 fps
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  (void)appstate; // unused for now

  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  app_destroy(appstate);

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
