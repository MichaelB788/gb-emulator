#include "app.h"
#include "gameboy.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, const char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "main: No ROM provided!\n");
    return EXIT_FAILURE;
  }

  // SDL subsystems initialization
  if (!SDL_Init(SDL_INIT_EVENTS)) {
    fprintf(stderr, "main: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  // Parse the ROM path
  char rom_path[FILENAME_MAX];
  strncpy(rom_path, argv[1], FILENAME_MAX);

  // Parse program arguments
  enum gb_debug_option opt = GB_DEBUG_ENABLE_NONE;
  for (int i = 2; i < argc; ++i) {
    if (strncmp(argv[i], "--brk", 5) == 0)
      opt = GB_DEBUG_ENABLE_BREAKPOINTS;
    else if (strncmp(argv[i], "--logv", 6) == 0)
      opt = GB_DEBUG_ENABLE_LOG_VERBOSE;
    else if (strncmp(argv[i], "--logb", 6) == 0)
      opt = GB_DEBUG_ENABLE_LOG_BRIEF;
    else
      fprintf(stderr, "Unknown flag %s\n", argv[i]);
  }

  // Create and run the app
  struct app app = {};
  if (app_create(&app, rom_path, opt))
    app_loop(&app);
  app_destroy(&app);
  SDL_Quit();

  return EXIT_SUCCESS;
}
