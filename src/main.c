#include "gameboy.h"
#include "optables.h"
#include <stdint.h>
#include <stdio.h>

void log_instruction(struct gameboy *gb, FILE *output) {
  const struct cpu cpu = gb->cpu;
  fprintf(output,
          "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X "
          "PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
          cpu.A, cpu.F, cpu.B, cpu.C, cpu.D, cpu.E, cpu.H, cpu.L, cpu.SP,
          cpu.PC, read_byte(gb, cpu.PC), read_byte(gb, cpu.PC + 1),
          read_byte(gb, cpu.PC + 2), read_byte(gb, cpu.PC + 3));
  fflush(output);
}

int main() {
  char full_path[sizeof(PROJECT_ROOT) + 64];
  snprintf(full_path, sizeof(full_path),
           "%s/extern/gb-test-roms/cpu_instrs/cpu_instrs.gb", PROJECT_ROOT);
  FILE *log_file = fopen("log.txt", "w");
  if (!log_file) {
    perror("Couldn't open log.txt");
    return 1;
  }

  struct gameboy gb = {0};
  if (init_gameboy(&gb, full_path)) {
    while (gb.state == GB_RUNNING) {
      gb.opcode = read_byte(&gb, gb.cpu.PC);
      log_instruction(&gb, log_file);
      ++gb.cpu.PC;
      int cycles = unprefixed_ins[gb.opcode](&gb);
    }
    close_gameboy(&gb);
  }
  fclose(log_file);
}
