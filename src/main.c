#include "appstate.h"
#include "gameboy.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_main.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  if (argc < 2) {
    SDL_SetError("No ROM given");
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_EVENTS)) {
    return SDL_APP_FAILURE;
  }

  *appstate = malloc(sizeof(struct appstate));
  if (*appstate == NULL) {
    SDL_SetError("Could not allocate memory for appstate");
    return SDL_APP_FAILURE;
  }

  struct appstate *state = *appstate;
  if (!init_gameboy(&state->gb, argv[1])) {
    SDL_SetError("Could not initialize the gameboy");
    return SDL_APP_FAILURE;
  }

  if (argv[2] && strncmp(argv[2], "--logging_enabled", 17) == 0) {
    state->log_file = fopen("log.txt", "w");
    if (!state->log_file) {
      perror("Could not open log file");
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  struct appstate *state = appstate;
  int cycles = 0;

  cycles += cpu_step(state);
  cycles += handle_interrupts(&state->gb);

  timer_tick(&state->gb.timer, cycles, &state->gb.interrupt);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  default:
    break;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  if (appstate) {
    struct appstate *state = appstate;
    close_gameboy(&state->gb);
    if (state->log_file) {
      fclose(state->log_file);
    }
    free(appstate);
  }

  if (result == SDL_APP_SUCCESS) {
    printf("App exited successfully.\n");
  } else if (result == SDL_APP_FAILURE) {
    fprintf(stderr, "An error occurred: %s\n", SDL_GetError());
  }
}
