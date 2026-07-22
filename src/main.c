#include "appstate.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  if (!SDL_Init(SDL_INIT_EVENTS)) {
    return SDL_APP_FAILURE;
  }

  *appstate = create_app(
      argv[1], argv[2] && strncmp(argv[2], "--logging_enabled", 17) == 0);
  if (*appstate == NULL) {
    SDL_SetError("Failed to create app");
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  struct appstate *state = appstate;

  const uint8_t cycles = cpu_step(&state->gb.cpu);
  timer_tick(&state->gb.bus.timer, cycles, &state->gb.bus.interrupt);

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
  destroy_app(appstate);

  if (result == SDL_APP_SUCCESS) {
    printf("App exited successfully.\n");
  } else if (result == SDL_APP_FAILURE) {
    fprintf(stderr, "An error occurred: %s\n", SDL_GetError());
  }
}
