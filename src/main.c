#include "core/gameboy.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

int main(int argc, const char **argv) {
  if (argc > 1) {
    GameBoy gb;
    init_gameboy(&gb, argv[1]);
    close_gameboy(&gb);
  } else {
    fprintf(stderr, "Usage: ./Gameboy <path/to/rom>");
  }
}
