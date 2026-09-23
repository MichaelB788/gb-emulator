#include "app.h"
#include "gameboy.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static bool str_matches_opt(const char *str, const char *opt) {
  char shorthand[] = {opt[1], opt[2]};
  return strncmp(str, opt, strlen(opt)) == 0 || strncmp(str, shorthand, 2) == 0;
}

static enum gb_dbg_opt parse_gb_opt_from_str(const char *str) {
  if (str) {
    if (str_matches_opt(str, "--interactive")) {
      return GB_OPT_INTERACTIVE_DEBUGGING;
    } else if (str_matches_opt(str, "--verbose")) {
      return GB_OPT_VERBOSE_LOGGING;
    } else if (str_matches_opt(str, "--brief")) {
      return GB_OPT_BRIEF_LOGGING;
    } else {
      puts("Unknown option given. Ignoring.");
      return GB_OPT_NONE;
    }
  }
  return GB_OPT_NONE;
}

int main(int argc, const char *argv[]) {
  if (argc < 2 || str_matches_opt(argv[1], "--help")) {
    printf("Usage: ./GameBoy [ROM_FILE] [OPTION]\n"
           "\n"
           "-v, --verbose       verbose logging\n"
           "-b, --brief         brief logging\n"
           "-i, --interactive   interactive debugging\n"
           "-h, --help          help\n");
    return EXIT_SUCCESS;
  }

  // SDL subsystems initialization
  if (!SDL_Init(SDL_INIT_EVENTS)) {
    fprintf(stderr, "main: %s\n", SDL_GetError());
    return EXIT_FAILURE;
  }

  // Parse the ROM path
  char rom_path[FILENAME_MAX];
  strncpy(rom_path, argv[1], FILENAME_MAX);

  // Create and run the app
  struct app app = {};
  if (app_create(&app, rom_path, parse_gb_opt_from_str(argv[2])))
    app_loop(&app);
  app_destroy(&app);
  SDL_Quit();

  return EXIT_SUCCESS;
}
