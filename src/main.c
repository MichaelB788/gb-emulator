#include "core/gameboy.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, const char **argv) {
  if (argc > 1) {
    GameBoy gameboy = {0};
    // TODO: Sanitize path
    if (init_gameboy(&gameboy, argv[1])) {
      run_gameboy(&gameboy);
    }
    close_gameboy(&gameboy);
  } else {
    fprintf(stderr, "Usage: ./Gameboy <path/to/rom>");
  }
}
