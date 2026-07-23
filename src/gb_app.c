#include "gb_app.h"
#include "gb_emulator.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void app_init(struct gb_app *app, const char *path_to_rom,
              bool logging_enabled) {
  app->status = APP_RUNNING;

  if (logging_enabled) {
    app->log_file = fopen("log.txt", "w");
    if (!app->log_file) {
      app->msg = strerror(errno);
    }
  }

  if (!SDL_Init(SDL_INIT_EVENTS)) {
    app->status = APP_FAILURE;
    app->msg = SDL_GetError();
  }

  if (!emu_init(&app->gb, path_to_rom, app->log_file)) {
    app->status = APP_FAILURE;
    app->msg = "Could not init emulator";
  }
}

void app_handle_events(struct gb_app *app) {
  while (SDL_PollEvent(&app->event)) {
    switch (app->event.type) {
    case SDL_EVENT_QUIT:
      app->status = APP_SUCCESS;
      break;
    default:
      break;
    }
  }
}

void app_run(struct gb_app *app) {
  while (app->status == APP_RUNNING) {
    app_handle_events(app);
    emu_tick(&app->gb);
    // TODO: rendering, keyboard input, etc.
  }
}

void app_close(struct gb_app *app) {
  emu_close(&app->gb);
  if (app->log_file) {
    fclose(app->log_file);
  }

  if (app->status == APP_SUCCESS) {
    printf("App exited successfully.\n");
  } else if (app->status == APP_FAILURE) {
    fprintf(stderr, "An error occurred: %s\n", app->msg);
  }
}
