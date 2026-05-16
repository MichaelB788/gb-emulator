#include "core/instructions.h"
#include "core/bus.h"
#include "core/cpu.h"
#include "core/instruction_set.h"
#include "util/bitwise.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Load Instructions
void ld_r8_r8(CPU *cpu) { *r8_dest(cpu) = *r8(cpu); }

void ld_r8_n8(CPU *cpu) { *r8_dest(cpu) = fetch_byte(cpu); }

void ld_r16_n16(CPU *cpu) { *r16(cpu) = fetch_word(cpu); }

void ld_mem_hl_r8(CPU *cpu) { write_hl(cpu, *r8(cpu)); }

void ld_mem_hl_n8(CPU *cpu) { write_hl(cpu, fetch_byte(cpu)); }

void ld_r8_mem_hl(CPU *cpu) { *r8_dest(cpu) = read_hl(cpu); }

void ld_mem_r16_a(CPU *cpu) { write_byte(cpu->bus, *r16(cpu), cpu->A); }

void ld_mem_n16_a(CPU *cpu) { write_byte(cpu->bus, fetch_word(cpu), cpu->A); }

void ldh_mem_n8_a(CPU *cpu) {
  write_byte(cpu->bus, (uint16_t)(0xFF00 | fetch_byte(cpu)), cpu->A);
}

void ldh_mem_c_a(CPU *cpu) {
  write_byte(cpu->bus, (uint16_t)(0xFF00 | cpu->C), cpu->A);
}

void ld_a_mem_r16(CPU *cpu) { cpu->A = read_byte(cpu->bus, *r16(cpu)); }

void ld_a_mem_n16(CPU *cpu) { cpu->A = read_byte(cpu->bus, fetch_word(cpu)); }

void ldh_a_mem_n8(CPU *cpu) {
  cpu->A = read_byte(cpu->bus, (uint16_t)(0xFF00 | fetch_byte(cpu)));
}

void ldh_a_mem_c(CPU *cpu) {
  cpu->A = read_byte(cpu->bus, (uint16_t)(0xFF00 | cpu->C));
}

void ld_mem_hli_a(CPU *cpu) { write_byte(cpu->bus, cpu->HL++, cpu->A); }

void ld_mem_hld_a(CPU *cpu) { write_byte(cpu->bus, cpu->HL--, cpu->A); }

void ld_a_mem_hli(CPU *cpu) { cpu->A = read_byte(cpu->bus, cpu->HL++); }

void ld_a_mem_hld(CPU *cpu) { cpu->A = read_byte(cpu->bus, cpu->HL--); }

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

void adc_a_n8(CPU *cpu) { ADC(cpu, fetch_byte(cpu)); }

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

void add_a_n8(CPU *cpu) { ADD(cpu, fetch_byte(cpu)); }

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

void cp_a_n8(CPU *cpu) { CP(cpu, fetch_byte(cpu)); }

void DEC(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = r8 - 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (r8 & 0xF) == 0x0);

  *operand = result;
}

void dec_r8(CPU *cpu) { DEC(cpu, r8_dest(cpu)); }

void dec_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  DEC(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void INC(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = r8 + 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (r8 & 0xF) == 0xF);

  *operand = result;
}

void inc_r8(CPU *cpu) { INC(cpu, r8_dest(cpu)); }

void inc_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  INC(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void SBC(CPU *cpu, const uint8_t operand) {
  const uint8_t carry = get_flag(cpu, FLAG_C);
  const uint8_t A = cpu->A;

  const uint8_t result = A - (operand + carry);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, true);
  set_flag(cpu, FLAG_H, (A & 0xF) < (operand & 0xF) + carry);
  set_flag(cpu, FLAG_C, A < operand + carry);

  cpu->A = result;
}

void sbc_a_r8(CPU *cpu) { SBC(cpu, *r8(cpu)); }

void sbc_a_mem_hl(CPU *cpu) { SBC(cpu, read_hl(cpu)); }

void sbc_a_n8(CPU *cpu) { SBC(cpu, fetch_byte(cpu)); }

void SUB(CPU *cpu, const uint8_t operand) {
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

void sub_a_n8(CPU *cpu) { SUB(cpu, fetch_byte(cpu)); }

void add_hl_r16(CPU *cpu) {
  const uint16_t HL = cpu->HL;
  const uint16_t operand = *r16(cpu);

  const uint32_t sum = HL + operand;

  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  set_flag(cpu, FLAG_C, sum > 0xFFFF);

  cpu->HL = (uint16_t)sum;
}

void dec_r16(CPU *cpu) { --(*r16(cpu)); }

void inc_r16(CPU *cpu) { ++(*r16(cpu)); }

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

void and_a_n8(CPU *cpu) { AND(cpu, fetch_byte(cpu)); }

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

void or_a_n8(CPU *cpu) { OR(cpu, fetch_byte(cpu)); }

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

void xor_a_n8(CPU *cpu) { XOR(cpu, fetch_byte(cpu)); }

void BIT(CPU *cpu, const uint8_t operand) {
  set_flag(cpu, FLAG_Z, !get_bit(operand, op_y(cpu->opcode)));
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, true);
}

void bit_r8(CPU *cpu) { BIT(cpu, *r8(cpu)); }

void bit_mem_hl(CPU *cpu) { BIT(cpu, read_hl(cpu)); }

void res_r8(CPU *cpu) { set_bit(r8(cpu), op_y(cpu->opcode), false); }

void res_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  set_bit(&hl_ind, op_y(cpu->opcode), false);
  write_hl(cpu, hl_ind);
}

void set_r8(CPU *cpu) { set_bit(r8(cpu), op_y(cpu->opcode), true); }

void set_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  set_bit(&hl_ind, op_y(cpu->opcode), true);
  write_hl(cpu, hl_ind);
}

void RL(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = r8 << 1 | get_flag(cpu, FLAG_C);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, get_bit(r8, 7));

  *operand = result;
}

void rl_r8(CPU *cpu) { RL(cpu, r8(cpu)); }

void rl_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  RL(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void rla(CPU *cpu) {
  RL(cpu, &cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

void RLC(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const bool bit_7 = get_bit(r8, 7);
  const uint8_t result = (r8 << 1) | bit_7;

  set_flag(cpu, FLAG_Z, r8 == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, bit_7);

  *operand = result;
}

void rlc_r8(CPU *cpu) { RLC(cpu, r8(cpu)); }

void rlc_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  RLC(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void rlca(CPU *cpu) {
  RLC(cpu, &cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

void RR(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = (get_flag(cpu, FLAG_C) << 7) | (r8 >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, r8 & 1);

  *operand = result;
}

void rr_r8(CPU *cpu) { RR(cpu, r8(cpu)); }

void rr_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  RR(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void rra(CPU *cpu) {
  RR(cpu, &cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

void RRC(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t bit_0 = r8 & 1;
  const uint8_t result = (bit_0 << 7) | (r8 >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, bit_0);

  *operand = result;
}

void rrc_r8(CPU *cpu) { RRC(cpu, r8(cpu)); }

void rrc_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  RRC(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void rrca(CPU *cpu) {
  RRC(cpu, &cpu->A);
  set_flag(cpu, FLAG_Z, false);
}

void SLA(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = r8 << 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, get_bit(r8, 7));

  *operand = result;
}

void sla_r8(CPU *cpu) { SLA(cpu, r8(cpu)); }

void sla_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  SLA(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void SRA(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = (r8 & 0x80) | (r8 >> 1);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, r8 & 1);

  *operand = result;
}

void sra_r8(CPU *cpu) { SRA(cpu, r8(cpu)); }

void sra_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  SRA(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void SRL(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = r8 >> 1;

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, r8 & 1);

  *operand = result;
}

void srl_r8(CPU *cpu) { SRL(cpu, r8(cpu)); }

void srl_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  SRL(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void SWAP(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = (r8 << 4) | (r8 >> 4);

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, false);

  *operand = result;
}

void swap_r8(CPU *cpu) { SWAP(cpu, r8(cpu)); }

void swap_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  SWAP(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void call_a16(CPU *cpu) {
  const uint16_t jmp_addr = fetch_word(cpu);
  write_byte(cpu->bus, --cpu->SP, cpu->PC >> 8);
  write_byte(cpu->bus, --cpu->SP, cpu->PC & 0xFF);
  cpu->PC = jmp_addr;
}

void call_cc_a16(CPU *cpu) {
  const uint16_t jmp_addr = fetch_word(cpu);
  if (check_cc(cpu)) {
    write_byte(cpu->bus, --cpu->SP, cpu->PC >> 8);
    write_byte(cpu->bus, --cpu->SP, cpu->PC & 0xFF);
    cpu->PC = jmp_addr;
    cpu->cycles_taken += 12;
  }
}

void jp_hl(CPU *cpu) { cpu->PC = cpu->HL; }

void jp_a16(CPU *cpu) { cpu->PC = fetch_word(cpu); }

void jp_cc_a16(CPU *cpu) {
  const uint16_t jmp_addr = fetch_word(cpu);
  if (check_cc(cpu)) {
    cpu->PC = jmp_addr;
    cpu->cycles_taken += 4;
  }
}

void jr_e8(CPU *cpu) { cpu->PC += (int8_t)fetch_byte(cpu); }

void jr_cc_e8(CPU *cpu) {
  const uint16_t jmp_addr = cpu->PC + (int8_t)fetch_byte(cpu);
  if (check_cc(cpu)) {
    cpu->PC = jmp_addr;
    cpu->cycles_taken += 4;
  }
}

void ret(CPU *cpu) {
  const uint8_t lo = read_byte(cpu->bus, cpu->SP++);
  const uint8_t hi = read_byte(cpu->bus, cpu->SP++);
  cpu->PC = (uint16_t)hi << 8 | lo;
}

void ret_cc(CPU *cpu) {
  if (check_cc(cpu)) {
    const uint8_t lo = read_byte(cpu->bus, cpu->SP++);
    const uint8_t hi = read_byte(cpu->bus, cpu->SP++);
    cpu->PC = (uint16_t)hi << 8 | lo;
    cpu->cycles_taken += 12;
  }
}

void reti(CPU *cpu) {
  const uint8_t lo = read_byte(cpu->bus, cpu->SP++);
  const uint8_t hi = read_byte(cpu->bus, cpu->SP++);
  cpu->PC = (uint16_t)hi << 8 | lo;
  cpu->IME = true;
}

void rst_vec(CPU *cpu) {
  write_byte(cpu->bus, --cpu->SP, cpu->PC >> 8);
  write_byte(cpu->bus, --cpu->SP, cpu->PC & 0xFF);
  cpu->PC = (uint16_t)(op_y(cpu->opcode) * 8);
}

// Carry flag instructions
void ccf(CPU *cpu) {
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, !get_flag(cpu, FLAG_C));
}

void scf(CPU *cpu) {
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, false);
  set_flag(cpu, FLAG_C, true);
}

// Stack manipulation instructions
void ADD_SP_e8(CPU *cpu, uint16_t *dest) {
  const uint16_t SP = cpu->SP;
  const uint8_t n8 = fetch_byte(cpu);
  const int32_t sum = SP + (int8_t)n8;

  set_flag(cpu, FLAG_Z, false);
  set_flag(cpu, FLAG_N, false);
  set_flag(cpu, FLAG_H, (SP & 0xF) + (n8 & 0xF) > 0xF);
  set_flag(cpu, FLAG_C, sum > 0xFF);

  *dest = (uint16_t)sum;
}

void add_sp_e8(CPU *cpu) { ADD_SP_e8(cpu, &cpu->SP); }

void ld_mem_n16_sp(CPU *cpu) {
  const uint16_t addr = fetch_word(cpu);
  write_byte(cpu->bus, addr, cpu->SP & 0xFF);
  write_byte(cpu->bus, addr + 1, cpu->SP >> 8);
}

void ld_hl_sp_e8(CPU *cpu) { ADD_SP_e8(cpu, &cpu->HL); }

void ld_sp_hl(CPU *cpu) { cpu->SP = cpu->HL; }

void pop_r16(CPU *cpu) {
  const uint8_t lo = read_byte(cpu->bus, cpu->SP++);
  const uint8_t hi = read_byte(cpu->bus, cpu->SP++);
  *r16(cpu) = (uint16_t)hi << 8 | lo;
}

void pop_af(CPU *cpu) {
  cpu->F = read_byte(cpu->bus, cpu->SP++) & 0xF0;
  cpu->A = read_byte(cpu->bus, cpu->SP++);
}

void push_r16(CPU *cpu) {
  const uint16_t operand = *r16(cpu);
  write_byte(cpu->bus, --cpu->SP, operand >> 8);
  write_byte(cpu->bus, --cpu->SP, operand & 0xFF);
}

void push_af(CPU *cpu) {
  write_byte(cpu->bus, --cpu->SP, cpu->A);
  write_byte(cpu->bus, --cpu->SP, cpu->F);
}

// Interrupt-related instructions
void halt(CPU *cpu) {
  // TODO: Proper halt behavior, halt bug.
  cpu->state = CPU_HALTED;
}

// Misc.
void daa(CPU *cpu) {
  const uint8_t A = cpu->A;
  uint8_t result = 0;

  if (get_flag(cpu, FLAG_N)) {
    uint8_t adjustment = 0;
    if (get_flag(cpu, FLAG_H)) {
      adjustment |= 0x6;
    }
    if (get_flag(cpu, FLAG_C)) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    uint8_t adjustment = 0;
    if (get_flag(cpu, FLAG_H) || (A & 0xF) > 0x9) {
      adjustment |= 0x6;
    }
    if (get_flag(cpu, FLAG_C) || A > 0x99) {
      adjustment |= 0x60;
      set_flag(cpu, FLAG_C, true);
    }
    result = A + adjustment;
  }

  set_flag(cpu, FLAG_Z, result == 0);
  set_flag(cpu, FLAG_H, false);

  cpu->A = result;
}

void prefix(CPU *cpu) {
  const uint8_t curr_op = fetch_byte(cpu);
  cpu->opcode = curr_op;

  Instruction ins = cb_optable[curr_op];
  cpu->cycles_taken = ins.cycles;
  ins.exec(cpu);

#ifndef NDEBUG
  log_ins(cpu, &ins);
#endif
}

void nop(CPU *cpu) {};

void stop_n8(CPU *cpu) {
  (void)fetch_byte(cpu);
  cpu->state = CPU_STOPPED;
}

void illegal(CPU *cpu) {
  fprintf(stderr, "Illegal opcode encountered: 0x%X\n", cpu->opcode);
  cpu->state = CPU_STOPPED;
}

void di(CPU *cpu) { cpu->IME = false; }

void ei(CPU *cpu) { cpu->IME = true; }
