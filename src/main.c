#include "gameboy.h"
#include "optables.h"
#include <stdint.h>
#include <stdio.h>

void log_instruction(const struct cpu *cpu, const uint8_t opcode,
                     uint16_t pc_prev, FILE *output) {
  fprintf(output,
          "[%02X] B:%02x C:%02x D:%02x E:%02x H:%02x L:%02x A:%02x F:%02x "
          "PC:%04x SP:%04x %s\n",
          opcode, cpu->r8[REG_B], cpu->r8[REG_C], cpu->r8[REG_D],
          cpu->r8[REG_E], cpu->r8[REG_H], cpu->r8[REG_L], cpu->r8[REG_A],
          cpu->r8[REG_F], pc_prev, cpu->SP, unprefixed_mnemonic[opcode]);
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
      log_instruction(&gb.cpu, gb.opcode, gb.cpu.PC, log_file);
      gb.cpu.PC++;
      int cycles = unprefixed_ins[gb.opcode](&gb);
    }
    close_gameboy(&gb);
  }
  fclose(log_file);
}
