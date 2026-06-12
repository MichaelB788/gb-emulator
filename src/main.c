#include "gameboy.h"
#include <stdio.h>
#include <string.h>

int main() {
  char full_path[200] = PROJECT_ROOT;
  const char *test_rom = "/extern/gb-test-roms/cpu_instrs/cpu_instrs.gb";
  strcat(full_path, test_rom);

  GameBoy gameboy = {0};
  if (init_gameboy(&gameboy, full_path)) {
    run_gameboy(&gameboy);
  }
  close_gameboy(&gameboy);

  printf("Hello world\n");
}
