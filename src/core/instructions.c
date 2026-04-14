#include "core/cpu.h"
#include <stdint.h>

void ld_r8_r8(CPU *cpu) {
  uint8_t *dest = cpu->r8[op_y(cpu->IR)];
  uint8_t *src = cpu->r8[op_z(cpu->IR)];
  *dest = *src;
}

void ld_r8_mem_hl(CPU *cpu) {
  uint8_t *dest = cpu->r8[op_y(cpu->IR)];
  *dest = read_hl(cpu);
}

void halt(CPU *cpu) {}
