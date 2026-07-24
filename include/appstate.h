#pragma once
#include "gameboy.h"
#include <SDL3/SDL_events.h>

enum app_status { APP_RUNNING, APP_SUCCESS, APP_FAILURE };

struct appstate {
  enum app_status status;
  SDL_Event event;
  struct gb_emulator gb;
  FILE *log_file;
  const char *msg;
};

void app_init(struct appstate *app, const char *path_to_rom,
              bool logging_enabled);
void app_run(struct appstate *app);
void app_close(struct appstate *state);
