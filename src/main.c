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
    SDL_Quit();
    return EXIT_FAILURE;
  }

  // Parse the ROM path
  char rom_path[FILENAME_MAX];
  strncpy(rom_path, argv[1], FILENAME_MAX);

  // Parse program arguments
  enum gb_debug_option opt = GB_DEBUG_ENABLE_NONE;
  if (argc > 2) {
    if (strncmp(argv[2], "--breakpoint", 7) == 0)
      opt = GB_DEBUG_ENABLE_BREAKPOINTS;
    else if (strncmp(argv[2], "--logv", 6) == 0)
      opt = GB_DEBUG_ENABLE_LOG_VERBOSE;
    else if (strncmp(argv[2], "--logb", 6) == 0)
      opt = GB_DEBUG_ENABLE_LOG_BRIEF;
  }

  // Create and run the app
  struct app *app = app_malloc(rom_path, opt);
  if (app)
    app_loop(app);
  app_free(app);
  SDL_Quit();

  return EXIT_SUCCESS;
}
