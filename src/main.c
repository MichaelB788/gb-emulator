#include "gameboy.h"
#include <stdint.h>

int main(const int argc, const char **argv) {
  if (argc < 2) {
    fprintf(stderr, "No arguments given, aborting.\n");
    return 1;
  }

  struct gameboy gb = {0};
  if (init_gameboy(&gb, argv[1])) {
    run_gameboy_loop(&gb);
    close_gameboy(&gb);
  } else {
    fprintf(stderr, "Could not initialize GameBoy.\n");
  }
}
