#include "core/cpu.h"
#include "core/instruction_set.h"
#include "util/bitwise.h"
#include <assert.h>
#include <endian.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void init_cpu(CPU *cpu, Bus *bus) {
  cpu->r8[0] = &cpu->B;
  cpu->r8[1] = &cpu->C;
  cpu->r8[2] = &cpu->D;
  cpu->r8[3] = &cpu->E;
  cpu->r8[4] = &cpu->H;
  cpu->r8[5] = &cpu->L;
  cpu->r8[6] = NULL;
  cpu->r8[7] = &cpu->A;

  cpu->r16[0] = &cpu->BC;
  cpu->r16[1] = &cpu->DE;
  cpu->r16[2] = &cpu->HL;
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
    fprintf(
        output_file,
        "0x%02X: A:%02X F:%02X BC:%04X DE:%04X HL:%04X PC:%04X SP:%04X %s\n",
        cpu->opcode, cpu->A, cpu->F, cpu->BC, cpu->DE, cpu->HL, cpu->PC - 1,
        cpu->SP, ins->name);
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

uint8_t op_y(uint8_t op) { return (op >> 3) & 0x7; }

uint8_t op_z(uint8_t op) { return op & 0x7; }

uint8_t *r8(CPU *cpu) {
  uint8_t i = op_z(cpu->opcode);
  assert(i != 6);
  return cpu->r8[i];
}

uint8_t *r8_dest(CPU *cpu) {
  uint8_t i = op_y(cpu->opcode);
  assert(i != 6);
  return cpu->r8[i];
}

uint16_t *r16(CPU *cpu) { return cpu->r16[op_y(cpu->opcode) >> 1]; }

uint8_t read_n8(CPU *cpu) { return read_byte(cpu->bus, cpu->PC++); }

uint8_t read_hl(CPU *cpu) { return read_byte(cpu->bus, cpu->HL); }

void write_hl(CPU *cpu, uint8_t val) { write_byte(cpu->bus, cpu->HL, val); }

void set_flag(CPU *cpu, Flag flag, bool val) {
  set_bit(&cpu->F, (uint8_t)flag, val);
}

bool get_flag(CPU *cpu, Flag flag) { return get_bit(cpu->F, (uint8_t)flag); }
