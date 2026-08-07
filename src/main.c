#include <stdio.h>
#define SDL_MAIN_USE_CALLBACKS
#include "appstate.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_main.h>
#include <stdlib.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  *appstate = malloc(sizeof(struct appstate));

  // Appstate initialization
  if (!appstate_init(*appstate, argv)) {
    return SDL_APP_FAILURE;
  }

  // SDL subsystems initialization
  if (!SDL_Init(SDL_INIT_EVENTS)) {
    fprintf(stderr, "Could not initialize SDL: %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  appstate_update(appstate);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  default:
    return SDL_APP_CONTINUE;
  }
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  // Close emulator subsystems
  appstate_quit(appstate);
  free(appstate);

  // Print app result
  if (result == SDL_APP_SUCCESS) {
    printf("App exited successfully.\n");
  } else if (result == SDL_APP_FAILURE) {
    fprintf(stderr, "An error occurred.\n");
  }
}
