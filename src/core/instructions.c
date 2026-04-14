#include "core/bus.h"
#include "core/cpu.h"
#include <stdint.h>

// Load Instructions
void ld_r8_r8(CPU *cpu) {
  const uint8_t *src = cpu->r8[op_z(cpu->opcode)];
  uint8_t *dest = cpu->r8[op_y(cpu->opcode)];
  *dest = *src;
}

void ld_r8_n8(CPU *cpu) {
  uint8_t *dest = cpu->r8[op_y(cpu->opcode)];
  *dest = read_n8(cpu);
}

void ld_r16_n16(CPU *cpu) {
  uint16_t *dest = cpu->r16[op_y(cpu->opcode) >> 1];
  *dest = read_n16(cpu);
}

void ld_mem_hl_r8(CPU *cpu) {
  const uint8_t *src = cpu->r8[op_z(cpu->opcode)];
  write_hl(cpu, *src);
}

void ld_mem_hl_n8(CPU *cpu) { write_hl(cpu, read_n8(cpu)); }

void ld_r8_mem_hl(CPU *cpu) {
  uint8_t *dest = cpu->r8[op_y(cpu->opcode)];
  *dest = read_hl(cpu);
}

void ld_mem_r16_a(CPU *cpu) {
  const uint16_t addr = *cpu->r16[op_y(cpu->opcode) >> 1];
  write_byte(cpu->bus, addr, cpu->A);
}

void ld_mem_n16_a(CPU *cpu) {
  const uint16_t addr = read_n16(cpu);
  write_byte(cpu->bus, addr, cpu->A);
}

void ldh_mem_n8_a(CPU *cpu) {
  const uint16_t addr = 0xFF00 | read_n8(cpu);
  write_byte(cpu->bus, addr, cpu->A);
}

void ldh_mem_c_a(CPU *cpu) {
  const uint16_t addr = 0xFF00 | cpu->BC.byte.lo;
  write_byte(cpu->bus, addr, cpu->A);
}

void ld_a_mem_r16(CPU *cpu) {
  const uint16_t addr = *cpu->r16[op_y(cpu->opcode) >> 1];
  cpu->A = read_byte(cpu->bus, addr);
}

void ld_a_mem_n16(CPU *cpu) {
  const uint16_t addr = read_n16(cpu);
  cpu->A = read_byte(cpu->bus, addr);
}

void ldh_a_mem_n8(CPU *cpu) {
  const uint16_t addr = 0xFF00 | read_n8(cpu);
  cpu->A = read_byte(cpu->bus, addr);
}

void ldh_a_mem_c(CPU *cpu) {
  const uint16_t addr = 0xFF00 | cpu->BC.byte.lo;
  cpu->A = read_byte(cpu->bus, addr);
}

void ld_mem_hli_a(CPU *cpu) {
  write_hl(cpu, cpu->A);
  ++cpu->HL.word;
}

void ld_mem_hld_a(CPU *cpu) {
  write_hl(cpu, cpu->A);
  --cpu->HL.word;
}

void ld_a_mem_hli(CPU *cpu) {
  cpu->A = read_hl(cpu);
  ++cpu->HL.word;
}

void ld_a_mem_hld(CPU *cpu) {
  cpu->A = read_hl(cpu);
  --cpu->HL.word;
}

// 8-bit arithmetic
void ADC(CPU *cpu, const uint8_t operand) {
  const uint8_t carry = get_flag(cpu, FLAG_C);
  const uint8_t A = cpu->A;

  const uint16_t sum = A + operand + carry;
  const uint8_t result = (uint8_t)sum;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (A & 0xF) + (operand & 0xF) + carry > 0xF);
  set_flag(cpu, FLAG_C, sum > 0xFF);

  cpu->A = result;
}

void adc_a_r8(CPU *cpu) {
  const uint8_t operand = *cpu->r8[op_z(cpu->opcode)];
  ADC(cpu, operand);
}

void adc_a_mem_hl(CPU *cpu) {
  const uint8_t operand = read_hl(cpu);
  ADC(cpu, operand);
}

void adc_a_n8(CPU *cpu) {
  const uint8_t operand = read_n8(cpu);
  ADC(cpu, operand);
}

void ADD(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;

  const uint16_t sum = A + operand;
  const uint8_t result = (uint8_t)sum;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (A & 0xF) + (operand & 0xF) > 0xF);
  set_flag(cpu, FLAG_C, sum > 0xFF);

  cpu->A = result;
}

void add_a_r8(CPU *cpu) {
  const uint8_t operand = *cpu->r8[op_z(cpu->opcode)];
  ADD(cpu, operand);
}

void add_a_mem_hl(CPU *cpu) {
  const uint8_t operand = read_hl(cpu);
  ADD(cpu, operand);
}

void add_a_n8(CPU *cpu) {
  const uint8_t operand = read_n8(cpu);
  ADD(cpu, operand);
}

// Interrupt-related instructions
void halt(CPU *cpu) {}
