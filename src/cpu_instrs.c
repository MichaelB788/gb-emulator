#include "cpu_instrs.h"
#include "bitwise.h"
#include "cpu.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

uint8_t get_r8(CPU *cpu, uint8_t opcode_field) {
  if (opcode_field == 6)
    return read_hl(cpu);
  else
    return cpu->r8[opcode_field];
}

void set_r8(CPU *cpu, uint8_t opcode_field, uint8_t val) {
  if (opcode_field == 6)
    write_hl(cpu, val);
  else
    cpu->r8[opcode_field] = val;
}

uint16_t get_r16(CPU *cpu) {
  const uint16_t src = cpu->y_field >> 1;
  if (src == 3)
    return cpu->SP;
  else
    return get_r8_pair(cpu, (R16_Idx)src);
}

void set_r16(CPU *cpu, uint16_t val) {
  const uint16_t dest = cpu->y_field >> 1;
  if (dest == 3)
    cpu->SP = val;
  else
    set_r8_pair(cpu, (R16_Idx)dest, val);
}

uint16_t get_r16stk(CPU *cpu) {
  const uint16_t src = cpu->y_field >> 1;
  if (src == 3)
    return (uint16_t)cpu->r8[REG_A] << 8 | cpu->r8[REG_F];
  else
    return get_r8_pair(cpu, (R16_Idx)src);
}

void set_r16stk(CPU *cpu, uint16_t val) {
  const uint16_t dest = cpu->y_field >> 1;
  if (dest == 3) {
    cpu->r8[REG_A] = val >> 8;
    cpu->r8[REG_F] = val & 0xF0;
  } else {
    set_r8_pair(cpu, (R16_Idx)dest, val);
  }
}

uint16_t get_r16mem(CPU *cpu) {
  const uint8_t r16mem = cpu->y_field >> 1;
  if (r16mem == 0 || r16mem == 1) /* BC or DE */ {
    return get_r8_pair(cpu, (R16_Idx)r16mem);
  } else {
    const uint16_t ret = get_r8_pair(cpu, REG_HL);
    if (r16mem == 2) /* HLI */
      set_r8_pair(cpu, REG_HL, ret + 1);
    else /* HLD */
      set_r8_pair(cpu, REG_HL, ret - 1);
    return ret;
  }
}

// Load Instructions

void ld_r8_r8(CPU *cpu) {
  set_r8(cpu, cpu->y_field, get_r8(cpu, cpu->z_field));
}

void ld_r8_imm8(CPU *cpu) { set_r8(cpu, cpu->y_field, fetch_byte(cpu)); }

void ld_r16_n16(CPU *cpu) { set_r16(cpu, fetch_word(cpu)); }

void ld_mem_r16_a(CPU *cpu) {
  write_byte(cpu->gameboy, get_r16mem(cpu), cpu->r8[REG_A]);
}

void ld_mem_n16_a(CPU *cpu) {
  write_byte(cpu->gameboy, fetch_word(cpu), cpu->r8[REG_A]);
}

void ldh_mem_n8_a(CPU *cpu) {
  const uint16_t addr = 0xFF00 | fetch_byte(cpu);
  write_byte(cpu->gameboy, addr, cpu->r8[REG_A]);
}

void ldh_mem_c_a(CPU *cpu) {
  const uint16_t addr = 0xFF00 | cpu->r8[REG_C];
  write_byte(cpu->gameboy, addr, cpu->r8[REG_A]);
}

void ld_a_mem_r16(CPU *cpu) {
  cpu->r8[REG_A] = read_byte(cpu->gameboy, get_r16mem(cpu));
}

void ld_a_mem_n16(CPU *cpu) {
  cpu->r8[REG_A] = read_byte(cpu->gameboy, fetch_word(cpu));
}

void ldh_a_mem_n8(CPU *cpu) {
  const uint16_t addr = 0xFF00 | fetch_byte(cpu);
  cpu->r8[REG_A] = read_byte(cpu->gameboy, addr);
}

void ldh_a_mem_c(CPU *cpu) {
  const uint16_t addr = 0xFF00 | cpu->r8[REG_C];
  cpu->r8[REG_A] = read_byte(cpu->gameboy, addr);
}

// 8-bit arithmetic
void ADC(CPU *cpu, const uint8_t operand) {
  const uint8_t carry = is_c_set(cpu);
  const uint8_t A = cpu->r8[REG_A];

  const uint16_t sum = A + operand + carry;
  const uint8_t result = (uint8_t)sum;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, (A & 0xF) + (operand & 0xF) + carry > 0xF);
  set_c(cpu, sum > 0xFF);

  cpu->r8[REG_A] = result;
}

void adc_a_r8(CPU *cpu) { ADC(cpu, get_r8(cpu, cpu->z_field)); }

void adc_a_n8(CPU *cpu) { ADC(cpu, fetch_byte(cpu)); }

void ADD(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint16_t sum = A + operand;
  const uint8_t result = (uint8_t)sum;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, (A & 0xF) + (operand & 0xF) > 0xF);
  set_c(cpu, sum > 0xFF);

  cpu->r8[REG_A] = result;
}

void add_a_r8(CPU *cpu) { ADD(cpu, get_r8(cpu, cpu->z_field)); }

void add_a_n8(CPU *cpu) { ADD(cpu, fetch_byte(cpu)); }

void CP(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  set_z(cpu, A - operand == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF));
  set_c(cpu, A < operand);
}

void cp_a_r8(CPU *cpu) { CP(cpu, get_r8(cpu, cpu->z_field)); }

void cp_a_n8(CPU *cpu) { CP(cpu, fetch_byte(cpu)); }

void dec_r8(CPU *cpu) {
  const uint8_t dest = cpu->y_field;
  const uint8_t operand = get_r8(cpu, dest);
  const uint8_t result = operand - 1;

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (operand & 0xF) == 0x0);

  set_r8(cpu, dest, result);
}

void inc_r8(CPU *cpu) {
  const uint8_t dest = cpu->y_field;
  const uint8_t operand = get_r8(cpu, dest);
  const uint8_t result = operand + 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, (operand & 0xF) == 0xF);

  set_r8(cpu, dest, result);
}

void SBC(CPU *cpu, const uint8_t operand) {
  const uint8_t carry = is_c_set(cpu);
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A - (operand + carry);

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF) + carry);
  set_c(cpu, A < operand + carry);

  cpu->r8[REG_A] = result;
}

void sbc_a_r8(CPU *cpu) { SBC(cpu, get_r8(cpu, cpu->z_field)); }

void sbc_a_n8(CPU *cpu) { SBC(cpu, fetch_byte(cpu)); }

void SUB(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A - operand;

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF));
  set_c(cpu, A < operand);

  cpu->r8[REG_A] = result;
}

void sub_a_r8(CPU *cpu) { SUB(cpu, get_r8(cpu, cpu->z_field)); }

void sub_a_n8(CPU *cpu) { SUB(cpu, fetch_byte(cpu)); }

void add_hl_r16(CPU *cpu) {
  const uint16_t HL = get_r8_pair(cpu, REG_HL);
  const uint16_t operand = get_r16(cpu);

  const uint32_t sum = HL + operand;

  set_n(cpu, false);
  set_h(cpu, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  set_c(cpu, sum > 0xFFFF);

  set_r8_pair(cpu, REG_HL, sum);
}

void dec_r16(CPU *cpu) { set_r16(cpu, get_r16(cpu) - 1); }

void inc_r16(CPU *cpu) { set_r16(cpu, get_r16(cpu) + 1); }

void AND(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A & operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, true);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

void and_a_r8(CPU *cpu) { AND(cpu, get_r8(cpu, cpu->z_field)); }

void and_a_n8(CPU *cpu) { AND(cpu, fetch_byte(cpu)); }

void cpl(CPU *cpu) {
  cpu->r8[REG_A] = (uint8_t)~cpu->r8[REG_A];
  set_n(cpu, true);
  set_h(cpu, true);
}

void OR(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A | operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

void or_a_r8(CPU *cpu) { OR(cpu, get_r8(cpu, cpu->z_field)); }

void or_a_n8(CPU *cpu) { OR(cpu, fetch_byte(cpu)); }

void XOR(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A ^ operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

void xor_a_r8(CPU *cpu) { XOR(cpu, get_r8(cpu, cpu->z_field)); }

void xor_a_n8(CPU *cpu) { XOR(cpu, fetch_byte(cpu)); }

void bit_b3_r8(CPU *cpu) {
  set_z(cpu, !get_bit(get_r8(cpu, cpu->z_field), cpu->y_field));
  set_n(cpu, false);
  set_h(cpu, true);
}

void res_b3_r8(CPU *cpu) {
  const uint8_t dest = cpu->z_field;
  uint8_t operand = get_r8(cpu, dest);
  set_bit(&operand, cpu->y_field, false);
  set_r8(cpu, dest, operand);
}

void set_b3_r8(CPU *cpu) {
  const uint8_t dest = cpu->z_field;
  uint8_t operand = get_r8(cpu, dest);
  set_bit(&operand, cpu->y_field, true);
  set_r8(cpu, dest, operand);
}

void RL(CPU *cpu, uint8_t *operand) {}

void rl_r8(CPU *cpu) {
  const uint8_t dest = cpu->z_field;
  const uint8_t operand = get_r8(cpu, dest);
  const uint8_t result = operand << 1 | is_c_set(cpu);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  set_r8(cpu, dest, result);
}

void rla(CPU *cpu) {
  const uint8_t A = cpu->r8[REG_A];
  const uint8_t result = A << 1 | is_c_set(cpu);

  set_z(cpu, false);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, A & 0x80);

  cpu->r8[REG_A] = result;
}

void rlc_r8(CPU *cpu) {
  const uint8_t dest = cpu->z_field;
  const uint8_t operand = get_r8(cpu, dest);
  const uint8_t result = operand << 1 | is_c_set(cpu);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  set_r8(cpu, dest, result);
}

void rlca(CPU *cpu) {
  const uint8_t A = cpu->r8[REG_A];
  const uint8_t result = A << 1 | is_c_set(cpu);

  set_z(cpu, false);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, A & 0x80);

  cpu->r8[REG_A] = result;
}

void RR(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = (is_c_set(cpu) << 7) | (r8 >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, r8 & 1);

  *operand = result;
}

void rr_r8(CPU *cpu) { RR(cpu, r8(cpu)); }

void rr_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  RR(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void rra(CPU *cpu) {
  RR(cpu, &cpu->r8[REG_A]);
  set_z(cpu, false);
}

void RRC(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t bit_0 = r8 & 1;
  const uint8_t result = (bit_0 << 7) | (r8 >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, bit_0);

  *operand = result;
}

void rrc_r8(CPU *cpu) { RRC(cpu, r8(cpu)); }

void rrc_mem_hl(CPU *cpu) {
  uint8_t hl_ind = read_hl(cpu);
  RRC(cpu, &hl_ind);
  write_hl(cpu, hl_ind);
}

void rrca(CPU *cpu) {
  RRC(cpu, &cpu->r8[REG_A]);
  set_z(cpu, false);
}

void SLA(CPU *cpu, uint8_t *operand) {
  const uint8_t r8 = *operand;
  const uint8_t result = r8 << 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, get_bit(r8, 7));

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

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, r8 & 1);

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

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, r8 & 1);

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

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, false);

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
  write_byte(cpu->gameboy, --cpu->SP, cpu->PC >> 8);
  write_byte(cpu->gameboy, --cpu->SP, cpu->PC & 0xFF);
  cpu->PC = jmp_addr;
}

void call_cc_a16(CPU *cpu) {
  const uint16_t jmp_addr = fetch_word(cpu);
  if (check_cc(cpu)) {
    write_byte(cpu->gameboy, --cpu->SP, cpu->PC >> 8);
    write_byte(cpu->gameboy, --cpu->SP, cpu->PC & 0xFF);
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
  const int8_t jmp_addr = (int8_t)fetch_byte(cpu);
  if (check_cc(cpu)) {
    cpu->PC += jmp_addr;
    cpu->cycles_taken += 4;
  }
}

void ret(CPU *cpu) {
  const uint8_t lo = read_byte(cpu->gameboy, cpu->SP++);
  const uint8_t hi = read_byte(cpu->gameboy, cpu->SP++);
  cpu->PC = (uint16_t)hi << 8 | lo;
}

void ret_cc(CPU *cpu) {
  if (check_cc(cpu)) {
    const uint8_t lo = read_byte(cpu->gameboy, cpu->SP++);
    const uint8_t hi = read_byte(cpu->gameboy, cpu->SP++);
    cpu->PC = (uint16_t)hi << 8 | lo;
    cpu->cycles_taken += 12;
  }
}

void reti(CPU *cpu) {
  const uint8_t lo = read_byte(cpu->gameboy, cpu->SP++);
  const uint8_t hi = read_byte(cpu->gameboy, cpu->SP++);
  cpu->PC = (uint16_t)hi << 8 | lo;
  cpu->IME = true;
}

void rst_vec(CPU *cpu) {
  write_byte(cpu->gameboy, --cpu->SP, cpu->PC >> 8);
  write_byte(cpu->gameboy, --cpu->SP, cpu->PC & 0xFF);
  cpu->PC = (uint16_t)(op_y(cpu->opcode) * 8);
}

// Carry flag instructions
void ccf(CPU *cpu) {
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, !is_c_set(cpu));
}

void scf(CPU *cpu) {
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, true);
}

// Stack manipulation instructions
void ADD_SP_e8(CPU *cpu, uint16_t *dest) {
  const uint16_t SP = cpu->SP;
  const uint8_t n8 = fetch_byte(cpu);
  const int32_t sum = SP + (int8_t)n8;

  set_z(cpu, false);
  set_n(cpu, false);
  set_h(cpu, (SP & 0xF) + (n8 & 0xF) > 0xF);
  set_c(cpu, sum > 0xFF);

  *dest = (uint16_t)sum;
}

void add_sp_e8(CPU *cpu) { ADD_SP_e8(cpu, &cpu->SP); }

void ld_mem_n16_sp(CPU *cpu) {
  const uint16_t addr = fetch_word(cpu);
  write_byte(cpu->gameboy, addr, cpu->SP & 0xFF);
  write_byte(cpu->gameboy, addr + 1, cpu->SP >> 8);
}

void ld_hl_sp_e8(CPU *cpu) { ADD_SP_e8(cpu, &cpu->HL); }

void ld_sp_hl(CPU *cpu) { cpu->SP = cpu->HL; }

void pop_r16(CPU *cpu) {
  const uint8_t lo = read_byte(cpu->gameboy, cpu->SP++);
  const uint8_t hi = read_byte(cpu->gameboy, cpu->SP++);
  *r16(cpu) = (uint16_t)hi << 8 | lo;
}

void pop_af(CPU *cpu) {
  cpu->F = read_byte(cpu->gameboy, cpu->SP++) & 0xF0;
  cpu->r8[REG_A] = read_byte(cpu->gameboy, cpu->SP++);
}

void push_r16(CPU *cpu) {
  const uint16_t operand = *r16(cpu);
  write_byte(cpu->gameboy, --cpu->SP, operand >> 8);
  write_byte(cpu->gameboy, --cpu->SP, operand & 0xFF);
}

void push_af(CPU *cpu) {
  write_byte(cpu->gameboy, --cpu->SP, cpu->r8[REG_A]);
  write_byte(cpu->gameboy, --cpu->SP, cpu->F);
}

// Interrupt-related instructions
void halt(CPU *cpu) {
  // TODO: Proper halt behavior, halt bug.
  cpu->state = CPU_HALTED;
}

// Misc.
void daa(CPU *cpu) {
  const uint8_t A = cpu->r8[REG_A];
  uint8_t result = 0;

  if (get_flag(cpu, FLAG_N)) {
    uint8_t adjustment = 0;
    if (get_flag(cpu, FLAG_H)) {
      adjustment |= 0x6;
    }
    if (is_c_set(cpu)) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    uint8_t adjustment = 0;
    if (get_flag(cpu, FLAG_H) || (A & 0xF) > 0x9) {
      adjustment |= 0x6;
    }
    if (is_c_set(cpu) || A > 0x99) {
      adjustment |= 0x60;
      set_c(cpu, true);
    }
    result = A + adjustment;
  }

  set_z(cpu, result == 0);
  set_h(cpu, false);

  cpu->r8[REG_A] = result;
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
