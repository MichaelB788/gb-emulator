#include "core/instructions.h"
#include "core/bus.h"
#include "core/cpu.h"
#include "util/bitwise.h"
#include <stdint.h>
#include <stdio.h>

// Load Instructions
void ld_r8_r8(CPU *cpu) { *r8_dest(cpu) = *r8(cpu); }

void ld_r8_n8(CPU *cpu) { *r8_dest(cpu) = read_n8(cpu); }

void ld_r16_n16(CPU *cpu) { *r16(cpu) = read_n16(cpu); }

void ld_mem_hl_r8(CPU *cpu) { write_hl(cpu, *r8(cpu)); }

void ld_mem_hl_n8(CPU *cpu) { write_hl(cpu, read_n8(cpu)); }

void ld_r8_mem_hl(CPU *cpu) { *r8_dest(cpu) = read_hl(cpu); }

void ld_mem_r16_a(CPU *cpu) { write_byte(cpu->bus, *r16(cpu), cpu->A); }

void ld_mem_n16_a(CPU *cpu) { write_byte(cpu->bus, read_n16(cpu), cpu->A); }

void ldh_mem_n8_a(CPU *cpu) {
  write_byte(cpu->bus, (uint16_t)(0xFF00 | read_n8(cpu)), cpu->A);
}

void ldh_mem_c_a(CPU *cpu) {
  write_byte(cpu->bus, (uint16_t)(0xFF00 | cpu->BC.byte.lo), cpu->A);
}

void ld_a_mem_r16(CPU *cpu) { cpu->A = read_byte(cpu->bus, *r16(cpu)); }

void ld_a_mem_n16(CPU *cpu) { cpu->A = read_byte(cpu->bus, read_n16(cpu)); }

void ldh_a_mem_n8(CPU *cpu) {
  cpu->A = read_byte(cpu->bus, (uint16_t)(0xFF00 | read_n8(cpu)));
}

void ldh_a_mem_c(CPU *cpu) {
  cpu->A = read_byte(cpu->bus, (uint16_t)(0xFF00 | cpu->BC.byte.lo));
}

void ld_mem_hli_a(CPU *cpu) { write_byte(cpu->bus, cpu->HL.word++, cpu->A); }

void ld_mem_hld_a(CPU *cpu) { write_byte(cpu->bus, cpu->HL.word--, cpu->A); }

void ld_a_mem_hli(CPU *cpu) { cpu->A = read_byte(cpu->bus, cpu->HL.word++); }

void ld_a_mem_hld(CPU *cpu) { cpu->A = read_byte(cpu->bus, cpu->HL.word--); }

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

void adc_a_r8(CPU *cpu) { ADC(cpu, *r8(cpu)); }

void adc_a_mem_hl(CPU *cpu) { ADC(cpu, read_hl(cpu)); }

void adc_a_n8(CPU *cpu) { ADC(cpu, read_n8(cpu)); }

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

void add_a_r8(CPU *cpu) { ADD(cpu, *r8(cpu)); }

void add_a_mem_hl(CPU *cpu) { ADD(cpu, read_hl(cpu)); }

void add_a_n8(CPU *cpu) { ADD(cpu, read_n8(cpu)); }

void CP(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;

  const uint8_t result = A - operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF));
  set_flag(cpu, FLAG_C, A < operand);
}

void cp_a_r8(CPU *cpu) { CP(cpu, *r8(cpu)); }

void cp_a_mem_hl(CPU *cpu) { CP(cpu, read_hl(cpu)); }

void cp_a_n8(CPU *cpu) { CP(cpu, read_n8(cpu)); }

uint8_t DEC(CPU *cpu, const uint8_t operand) {
  const uint8_t result = operand - 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (operand & 0xF) == 0x0);

  return result;
}

void dec_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = DEC(cpu, *reg);
}

void dec_mem_hl(CPU *cpu) {
  const uint8_t result = DEC(cpu, read_hl(cpu));
  write_hl(cpu, result);
}

uint8_t INC(CPU *cpu, const uint8_t operand) {
  const uint8_t result = operand + 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (operand & 0xF) == 0xF);

  return result;
}

void inc_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = INC(cpu, *reg);
}

void inc_mem_hl(CPU *cpu) {
  const uint8_t result = INC(cpu, read_hl(cpu));
  write_hl(cpu, result);
}

void SBC(CPU *cpu, const uint8_t operand) {
  const uint8_t carry = get_flag(cpu, FLAG_C);
  const uint8_t A = cpu->A;

  const uint8_t result = A - operand - carry;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF) + carry);
  set_flag(cpu, FLAG_C, A < operand + carry);

  cpu->A = result;
}

void sbc_a_r8(CPU *cpu) { SBC(cpu, *r8(cpu)); }

void sbc_a_mem_hl(CPU *cpu) { SBC(cpu, read_hl(cpu)); }

void sbc_a_n8(CPU *cpu) { SBC(cpu, read_n8(cpu)); }

void SUB(CPU *cpu, uint8_t operand) {
  const uint8_t A = cpu->A;

  const uint8_t result = A - operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF));
  set_flag(cpu, FLAG_C, A < operand);

  cpu->A = result;
}

void sub_a_r8(CPU *cpu) { SUB(cpu, *r8(cpu)); }

void sub_a_mem_hl(CPU *cpu) { SUB(cpu, read_hl(cpu)); }

void sub_a_n8(CPU *cpu) { SUB(cpu, read_n8(cpu)); }

void add_hl_r16(CPU *cpu) {
  const uint16_t HL = cpu->HL.word;
  const uint16_t operand = *r16(cpu);

  const uint32_t sum = HL + operand;

  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  set_flag(cpu, FLAG_C, sum > 0xFFFF);

  cpu->HL.word = (uint16_t)sum;
}

void AND(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;

  const uint8_t result = A & operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, true);
  set_flag(cpu, FLAG_C, false);

  cpu->A = result;
}

void and_a_r8(CPU *cpu) { AND(cpu, *r8(cpu)); }

void and_a_mem_hl(CPU *cpu) { AND(cpu, read_hl(cpu)); }

void and_a_n8(CPU *cpu) { AND(cpu, read_n8(cpu)); }

void cpl(CPU *cpu) {
  cpu->A = (uint8_t)~cpu->A;
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, true);
}

void OR(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;

  const uint8_t result = A | operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, false);

  cpu->A = result;
}

void or_a_r8(CPU *cpu) { OR(cpu, *r8(cpu)); }

void or_a_mem_hl(CPU *cpu) { OR(cpu, read_hl(cpu)); }

void or_a_n8(CPU *cpu) { OR(cpu, read_n8(cpu)); }

void XOR(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->A;

  const uint8_t result = A ^ operand;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, false);

  cpu->A = result;
}

void xor_a_r8(CPU *cpu) { XOR(cpu, *r8(cpu)); }

void xor_a_mem_hl(CPU *cpu) { XOR(cpu, read_hl(cpu)); }

void xor_a_n8(CPU *cpu) { XOR(cpu, read_n8(cpu)); }

void BIT(CPU *cpu, const uint8_t operand) {
  const uint8_t bit_idx = op_y(cpu->opcode);

  set_flag(cpu, FLAG_Z, get_bit(operand, bit_idx));
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, true);
}

void bit_r8(CPU *cpu) { BIT(cpu, *r8(cpu)); }

void bit_mem_hl(CPU *cpu) { BIT(cpu, read_hl(cpu)); }

void res_r8(CPU *cpu) {
  const uint8_t bit_idx = op_y(cpu->opcode);
  set_bit(r8(cpu), bit_idx, false);
}

void res_mem_hl(CPU *cpu) {
  const uint8_t bit_idx = op_y(cpu->opcode);
  uint8_t hl_ind = read_hl(cpu);

  set_bit(&hl_ind, bit_idx, false);
  write_hl(cpu, hl_ind);
}

void set_r8(CPU *cpu) {
  const uint8_t bit_idx = op_y(cpu->opcode);
  set_bit(r8(cpu), bit_idx, true);
}

void set_mem_hl(CPU *cpu) {
  const uint8_t bit_idx = op_y(cpu->opcode);
  uint8_t hl_ind = read_hl(cpu);

  set_bit(&hl_ind, bit_idx, true);
  write_hl(cpu, hl_ind);
}

uint8_t RL(CPU *cpu, const uint8_t operand) {
  const uint8_t result = (operand << 1) | get_flag(cpu, FLAG_C);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, get_bit(operand, 7));

  return result;
}

void rl_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = RL(cpu, *reg);
}

void rl_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = RL(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

void rla(CPU *cpu) {
  cpu->A = RL(cpu, cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

uint8_t RLC(CPU *cpu, const uint8_t operand) {
  const bool bit_7 = get_bit(operand, 7);
  const uint8_t result = (operand << 1) | bit_7;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, bit_7);

  return result;
}

void rlc_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = RLC(cpu, *reg);
}

void rlc_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = RLC(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

void rlca(CPU *cpu) {
  cpu->A = RLC(cpu, cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

uint8_t RR(CPU *cpu, uint8_t operand) {
  const uint8_t result = (get_flag(cpu, FLAG_C) << 7) | (operand >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, result & 1);

  return result;
}

void rr_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = RR(cpu, *reg);
}

void rr_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = RR(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

void rra(CPU *cpu) {
  cpu->A = RR(cpu, cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

uint8_t RRC(CPU *cpu, uint8_t operand) {
  const uint8_t bit_0 = operand & 1;
  const uint8_t result = (bit_0 << 7) | (operand >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, bit_0);

  return result;
}

void rrc_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = RRC(cpu, *reg);
}

void rrc_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = RRC(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

void rrca(CPU *cpu) {
  cpu->A = RRC(cpu, cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

uint8_t SLA(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand << 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, get_bit(operand, 7));

  return result;
}

void sla_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = SLA(cpu, *reg);
}

void sla_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = SLA(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

uint8_t SRA(CPU *cpu, uint8_t operand) {
  const uint8_t result = (operand & 0x8) | (operand >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 1);

  return result;
}

void sra_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = SRA(cpu, *reg);
}

void sra_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = SRA(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

uint8_t SRL(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand >> 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, operand & 1);

  return result;
}

void srl_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = SRL(cpu, *reg);
}

void srl_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = SRL(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

uint8_t SWAP(CPU *cpu, uint8_t operand) {
  const uint8_t result = (operand << 4) | (operand >> 4);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, false);

  return result;
}

void swap_r8(CPU *cpu) {
  uint8_t *reg = r8(cpu);
  *reg = SWAP(cpu, *reg);
}

void swap_mem_hl(CPU *cpu) {
  const uint8_t hl_ind = SWAP(cpu, read_hl(cpu));
  write_hl(cpu, hl_ind);
}

void call_a16(CPU *cpu) {
  const uint16_t jmp_addr = read_n16(cpu);
  write_byte(cpu->bus, --cpu->SP, cpu->PC >> 8);
  write_byte(cpu->bus, --cpu->SP, cpu->PC & 0xFF);
  cpu->PC = jmp_addr;
}

void call_a16_cc(CPU *cpu) {
  const uint16_t jmp_addr = read_n16(cpu);
  if (check_cc(cpu)) {
    write_byte(cpu->bus, --cpu->SP, cpu->PC >> 8);
    write_byte(cpu->bus, --cpu->SP, cpu->PC & 0xFF);
    cpu->PC = jmp_addr;
    cpu->cycles_taken += 12;
  }
}

void jp_cc_a16(CPU *cpu) {
  const uint16_t jmp_addr = read_n16(cpu);
  if (check_cc(cpu)) {
    cpu->PC = jmp_addr;
    cpu->cycles_taken += 4;
  }
}

void jr_cc_a16(CPU *cpu) {
  const uint16_t jmp_addr = cpu->PC + (int8_t)read_n8(cpu);
  if (check_cc(cpu)) {
    cpu->PC = jmp_addr;
    cpu->cycles_taken += 4;
  }
}

void ret(CPU *cpu) {
  uint8_t lo = read_byte(cpu->bus, cpu->SP++);
  uint8_t hi = read_byte(cpu->bus, cpu->SP++);
  cpu->PC = (uint16_t)hi << 8 | lo;
}

void ret_cc(CPU *cpu) {
  if (check_cc(cpu)) {
    uint8_t lo = read_byte(cpu->bus, cpu->SP++);
    uint8_t hi = read_byte(cpu->bus, cpu->SP++);
    cpu->PC = (uint16_t)hi << 8 | lo;
    cpu->cycles_taken += 12;
  }
}

void reti(CPU *cpu) {
  uint8_t lo = read_byte(cpu->bus, cpu->SP++);
  uint8_t hi = read_byte(cpu->bus, cpu->SP++);
  cpu->PC = (uint16_t)hi << 8 | lo;
  cpu->IME = true;
}

void rst_vec(CPU *cpu) {
  write_byte(cpu->bus, --cpu->SP, cpu->PC >> 8);
  write_byte(cpu->bus, --cpu->SP, cpu->PC & 0xFF);
  cpu->PC = (uint16_t)(op_y(cpu->opcode) * 8);
}

// Interrupt-related instructions
void halt(CPU *cpu) {}

// Misc.
void illegal(CPU *cpu) {
  printf("Illegal opcode encountered: 0x%X", cpu->opcode);
}
