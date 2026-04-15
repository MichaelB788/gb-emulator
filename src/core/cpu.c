#include "core/cpu.h"
#include "core/instruction_set.h"
#include <assert.h>
#include <endian.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void init_cpu(CPU *cpu, Bus *bus) {
  cpu->bus = bus;

  cpu->r8[0] = &cpu->BC.byte.hi;
  cpu->r8[1] = &cpu->BC.byte.lo;
  cpu->r8[2] = &cpu->DE.byte.hi;
  cpu->r8[3] = &cpu->DE.byte.lo;
  cpu->r8[4] = &cpu->HL.byte.hi;
  cpu->r8[5] = &cpu->HL.byte.lo;
  cpu->r8[6] = NULL;
  cpu->r8[7] = &cpu->A;

  cpu->r16[0] = &cpu->BC.word;
  cpu->r16[1] = &cpu->DE.word;
  cpu->r16[2] = &cpu->HL.word;
  cpu->r16[3] = &cpu->SP;
}

uint8_t step(CPU *cpu) {
  const uint8_t curr_op = read_n8(cpu);
  cpu->opcode = curr_op;

  Instruction ins = optable[curr_op];
  cpu->cycles_taken = ins.cycles;
  ins.exec(cpu);

#ifndef NDEBUG
  log_ins(cpu, &ins);
#endif

  return cpu->cycles_taken;
}

void log_ins(CPU *cpu, Instruction *ins) {
  static FILE *output_file = NULL;
  if (output_file == NULL) {
    output_file = fopen("cpu_trace.txt", "w");
  }

  static const int max_entries = 10;
  static int entries = 0;
  if (entries < max_entries) {
    fprintf(output_file,
            "%02X: A:%02X F:%02X BC:%04X DE:%04X HL:%04X PC:%04X SP:%04X %s\n",
            cpu->opcode, cpu->A, cpu->F, cpu->BC.word, cpu->DE.word,
            cpu->HL.word, cpu->PC - 1, cpu->SP, ins->name);
    fflush(output_file);
  }
}

bool check_cc(CPU *cpu) {
  switch (op_y(cpu->opcode) & 0x3) {
  case 0: // NZ
    return !get_flag(cpu, FLAG_Z);
  case 1: // Z
    return get_flag(cpu, FLAG_Z);
  case 2: // NC
    return !get_flag(cpu, FLAG_C);
  case 3: // C
    return get_flag(cpu, FLAG_C);
  default:
    assert(false);
  }
}

uint16_t read_n16(CPU *cpu) {
  uint8_t lo = read_byte(cpu->bus, cpu->PC++);
  uint8_t hi = read_byte(cpu->bus, cpu->PC++);
  return (uint16_t)hi << 8 | lo;
}
