#include "app.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <stdio.h>
#include <stdlib.h>

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

  // Parse program arguments
  bool debug_enabled = false;
  for (int i = 1; i < argc; ++i) {
    debug_enabled = strncmp(argv[i], "--debug", 7) == 0;
  }

  // Create and run the app
  struct app *app = app_malloc(argv[1], debug_enabled);
  if (!app) {
    SDL_Quit();
    return EXIT_FAILURE;
  }
  app_loop(app);
  app_free(app);
  SDL_Quit();

  puts("Program success!");
  return EXIT_SUCCESS;
}
