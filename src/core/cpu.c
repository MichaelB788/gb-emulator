#include "core/cpu.h"

void init_cpu(CPU *cpu) {
  cpu->r8[0] = &cpu->BC.byte.hi;
  cpu->r8[1] = &cpu->BC.byte.lo;
  cpu->r8[2] = &cpu->DE.byte.hi;
  cpu->r8[3] = &cpu->DE.byte.lo;
  cpu->r8[4] = &cpu->HL.byte.hi;
  cpu->r8[5] = &cpu->HL.byte.lo;
  cpu->r8[6] = &cpu->F;
  cpu->r8[7] = &cpu->A;

  cpu->r16[0] = &cpu->BC.word;
  cpu->r16[1] = &cpu->DE.word;
  cpu->r16[2] = &cpu->HL.word;
  cpu->r16[3] = &cpu->PC;
}
