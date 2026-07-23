#pragma once
#include "gb_emulator.h"
#include <SDL3/SDL_events.h>

enum app_status { APP_RUNNING, APP_SUCCESS, APP_FAILURE };

struct gb_app {
  enum app_status status;
  SDL_Event event;
  struct gb_emulator gb;
  FILE *log_file;
  const char *msg;
};

void app_init(struct gb_app *app, const char *path_to_rom,
              bool logging_enabled);
void app_run(struct gb_app *app);
void app_close(struct gb_app *state);
