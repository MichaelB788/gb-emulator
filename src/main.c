#include "gameboy.h"
#include <stdio.h>
#include <string.h>

int main() {
  char full_path[200] = PROJECT_ROOT;
  const char *test_rom = "/extern/gb-test-roms/cpu_instrs/cpu_instrs.gb";
  strcat(full_path, test_rom);

  struct gameboy gb = {0};
  if (init_gameboy(&gb, full_path)) {
    run_gameboy(&gb);
  }
  close_gameboy(&gb);

  printf("Hello world\n");
}
