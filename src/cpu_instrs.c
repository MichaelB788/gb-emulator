#include "bitwise.h"
#include "cpu.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Helper functions
 */

uint8_t fetch_byte(GameBoy *gameboy) {
  return read_byte(gameboy, gameboy->cpu.PC++);
}

uint16_t fetch_word(GameBoy *gameboy) {
  const uint8_t lo = read_byte(gameboy, gameboy->cpu.PC++);
  const uint8_t hi = read_byte(gameboy, gameboy->cpu.PC++);
  return (uint16_t)hi << 8 | lo;
}

uint8_t read_hl(GameBoy *gameboy) {
  return read_byte(gameboy, get_r8_pair(&gameboy->cpu, REG_HL));
}

void write_hl(GameBoy *gameboy, uint8_t val) {
  write_byte(gameboy, get_r8_pair(&gameboy->cpu, REG_HL), val);
}

uint8_t get_r8(GameBoy *gameboy, uint8_t opcode_field) {
  if (opcode_field == 6)
    return read_hl(gameboy);
  else
    return gameboy->cpu.r8[opcode_field];
}

void set_r8(GameBoy *gameboy, uint8_t opcode_field, uint8_t val) {
  if (opcode_field == 6)
    write_hl(gameboy, val);
  else
    gameboy->cpu.r8[opcode_field] = val;
}

uint16_t get_r16(const CPU *cpu, uint8_t y_field) {
  const uint16_t src = y_field >> 1;
  if (src == 3)
    return cpu->SP;
  else
    return get_r8_pair(cpu, (R16_Idx)src);
}

void set_r16(CPU *cpu, uint8_t y_field, uint16_t val) {
  const uint16_t dest = y_field >> 1;
  if (dest == 3)
    cpu->SP = val;
  else
    set_r8_pair(cpu, (R16_Idx)dest, val);
}

uint16_t get_r16stk(const CPU *cpu, uint8_t y_field) {
  const uint16_t src = y_field >> 1;
  if (src == 3)
    return (uint16_t)cpu->r8[REG_A] << 8 | cpu->r8[REG_F];
  else
    return get_r8_pair(cpu, (R16_Idx)src);
}

void set_r16stk(CPU *cpu, uint8_t y_field, uint16_t val) {
  const uint16_t dest = y_field >> 1;
  if (dest == 3) {
    cpu->r8[REG_A] = val >> 8;
    cpu->r8[REG_F] = val & 0xF0;
  } else {
    set_r8_pair(cpu, (R16_Idx)dest, val);
  }
}

uint16_t get_r16mem(CPU *cpu, uint8_t y_field) {
  const uint8_t r16mem = y_field >> 1;
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

typedef enum { COND_NZ = 0, COND_Z = 1, COND_NC = 2, COND_C = 3 } Condition;
bool check_condition(CPU *cpu, Condition cond) {
  switch (cond) {
  case COND_NZ:
    return !is_z_set(cpu);
  case COND_Z: // Z
    return is_z_set(cpu);
  case COND_NC: // NC
    return !is_c_set(cpu);
  case COND_C: // C
    return is_c_set(cpu);
  }
}

void push_onto_stack(GameBoy *gameboy, uint16_t val) {
  write_byte(gameboy, --gameboy->cpu.SP, val >> 8);
  write_byte(gameboy, --gameboy->cpu.SP, val & 0xFF);
}

uint16_t pop_off_stack(GameBoy *gameboy) {
  const uint8_t lo = read_byte(gameboy, gameboy->cpu.SP++);
  const uint8_t hi = read_byte(gameboy, gameboy->cpu.SP++);
  return (uint16_t)hi << 8 | lo;
}

/**
 * 8-bit arithmetic instructions
 */

void ADD_n8(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint16_t sum = A + operand;
  const uint8_t result = (uint8_t)sum;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, (A & 0xF) + (operand & 0xF) > 0xF);
  set_c(cpu, sum > 0xFF);

  cpu->r8[REG_A] = result;
}

void ADC_n8(CPU *cpu, const uint8_t operand) {
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

void SUB_n8(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A - operand;

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF));
  set_c(cpu, A < operand);

  cpu->r8[REG_A] = result;
}

void SBC_n8(CPU *cpu, const uint8_t operand) {
  const uint8_t carry = is_c_set(cpu);
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A - (operand + carry);

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF) + carry);
  set_c(cpu, A < operand + carry);

  cpu->r8[REG_A] = result;
}

void CP_n8(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  set_z(cpu, A - operand == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF));
  set_c(cpu, A < operand);
}

uint8_t DEC_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand - 1;

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (operand & 0xF) == 0x0);

  return result;
}

uint8_t INC_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand + 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, (operand & 0xF) == 0xF);

  return result;
}

/**
 * 16-bit arithmetic instructions
 */

void ADD_r16(CPU *cpu, uint16_t operand) {
  const uint16_t HL = get_r8_pair(cpu, REG_HL);
  const uint32_t sum = HL + operand;

  set_n(cpu, false);
  set_h(cpu, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  set_c(cpu, sum > 0xFFFF);

  set_r8_pair(cpu, REG_HL, sum);
}

/**
 * Bitwise logic instructions
 */

void AND_n8(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A & operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, true);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

void XOR_n8(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A ^ operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

void OR_n8(CPU *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A | operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

/**
 * Bit flag instructions
 */

void BIT_b3_r8(CPU *cpu, uint8_t b3, uint8_t r8) {
  set_z(cpu, !get_bit(r8, b3));
  set_n(cpu, false);
  set_h(cpu, true);
}

/**
 * Bit shift instructions
 */

uint8_t RL_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | is_c_set(cpu);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  return result;
}

uint8_t RLC_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | is_c_set(cpu);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  return result;
}

uint8_t RR_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = (is_c_set(cpu) << 7) | (operand >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t RRC_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = ((operand & 1) << 7) | (operand >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t SLA_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand << 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  return result;
}

uint8_t SRA_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = (operand & 0x80) | (operand >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t SRL_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = operand >> 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t SWAP_r8(CPU *cpu, uint8_t operand) {
  const uint8_t result = (operand << 4) | (operand >> 4);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, false);

  return result;
}

/**
 * Misc.
 */

void DAA(CPU *cpu) {
  const uint8_t A = cpu->r8[REG_A];
  uint8_t result = 0;

  if (is_n_set(cpu)) {
    uint8_t adjustment = 0;
    if (is_h_set(cpu)) {
      adjustment |= 0x6;
    }
    if (is_c_set(cpu)) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    uint8_t adjustment = 0;
    if (is_h_set(cpu) || (A & 0xF) > 0x9) {
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

/**
 * Instruction decoding
 */

void execute_instruction(GameBoy *gb, uint8_t opcode) {
  uint8_t x = (opcode >> 6) & 0x3;
  uint8_t y = (opcode >> 3) & 0x7;
  uint8_t z = opcode & 0x7;

  switch (x) {
  case 0: /* Block 0 */ {
    switch (z) {
    case 0: {
      switch (z) {
      case 0: /* NOP */
        break;
      case 1: /* LD [a16] SP */
        write_byte(gb, fetch_word(gb), gb->cpu.SP);
        break;
      case 2: /* STOP */
        gb->state = GB_STOPPED;
        break;
      case 3: /* JR e8 */
        gb->cpu.PC += (int8_t)fetch_byte(gb);
        break;
      default: /* JR cc, e8 */ {
        const int8_t offset = fetch_byte(gb);
        if (check_condition(&gb->cpu, (Condition)(y & 0x3))) {
          gb->cpu.PC += offset;
          gb->cycles += 4;
        }
      } break;
      }
    } break;

    case 1: {
      if ((z & 1) == 0) /* LD r16, imm16 */
        set_r16(&gb->cpu, y, fetch_word(gb));
      else /* ADD HL, r16 */
        ADD_r16(&gb->cpu, get_r16(&gb->cpu, y));
    } break;

    case 2: {
      if ((z & 1) == 0) /* LD [r16mem], A */
        write_byte(gb, get_r16mem(&gb->cpu, y), gb->cpu.r8[REG_A]);
      else /* LD A, [r16mem] */
        gb->cpu.r8[REG_A] = read_byte(gb, get_r16mem(&gb->cpu, y));
    } break;

    case 3: {
      if ((z & 1) == 0) /* INC r16 */
        set_r16(&gb->cpu, y, get_r16(&gb->cpu, y) + 1);
      else /* DEC r16 */
        set_r16(&gb->cpu, y, get_r16(&gb->cpu, y) - 1);
    } break;

    case 4: /* INC r8 */ {
      set_r8(gb, y, INC_r8(&gb->cpu, get_r8(gb, y)));
    } break;

    case 5: /* DEC r8 */ {
      set_r8(gb, y, DEC_r8(&gb->cpu, get_r8(gb, y)));
    } break;

    case 6: /* LD r8, imm8 */ {
      set_r8(gb, y, fetch_byte(gb));
    } break;

    case 7: {
      switch (z) {
      case 0: /* RLCA */
        gb->cpu.r8[REG_A] = RLC_r8(&gb->cpu, gb->cpu.r8[REG_A]);
        set_z(&gb->cpu, false);
        break;
      case 1: /* RRCA */
        gb->cpu.r8[REG_A] = RRC_r8(&gb->cpu, gb->cpu.r8[REG_A]);
        set_z(&gb->cpu, false);
        break;
      case 2: /* RLA */
        gb->cpu.r8[REG_A] = RL_r8(&gb->cpu, gb->cpu.r8[REG_A]);
        set_z(&gb->cpu, false);
        break;
      case 3: /* RRA */
        gb->cpu.r8[REG_A] = RR_r8(&gb->cpu, gb->cpu.r8[REG_A]);
        set_z(&gb->cpu, false);
        break;
      case 4: /* DAA */
        DAA(&gb->cpu);
        break;
      case 5: /* CPL */
        gb->cpu.r8[REG_A] = ~gb->cpu.r8[REG_A];
        set_n(&gb->cpu, true);
        set_h(&gb->cpu, true);
        break;
      case 6: /* SCF */
        set_n(&gb->cpu, false);
        set_h(&gb->cpu, false);
        set_c(&gb->cpu, true);
        break;
      case 7: /* CCF */
        set_n(&gb->cpu, false);
        set_h(&gb->cpu, false);
        set_c(&gb->cpu, !is_c_set(&gb->cpu));
        break;
      }
    } break;
    }
  } break;

  case 1: /* Block 1: 8-bit register-to-register loads */ {
    if (y == 6 && z == 6) /* HALT */
      gb->state = GB_HALTED;
    else /* LD r8, r8 */
      set_r8(gb, y, get_r8(gb, z));
  } break;

  case 2: /* Block 2: 8-bit arithmetic */ {
    switch (y) {
    case 0: /* ADD r8 */
      ADD_n8(&gb->cpu, get_r8(gb, z));
      break;

    case 1: /* ADD r8 */
      ADC_n8(&gb->cpu, get_r8(gb, z));
      break;

    case 2: /* ADC r8 */
      SUB_n8(&gb->cpu, get_r8(gb, z));
      break;

    case 3: /* SUB r8 */
      SBC_n8(&gb->cpu, get_r8(gb, z));
      break;

    case 4: /* SBC r8 */
      AND_n8(&gb->cpu, get_r8(gb, z));
      break;

    case 5: /* XOR r8 */
      XOR_n8(&gb->cpu, get_r8(gb, z));
      break;

    case 6: /* OR r8 */
      OR_n8(&gb->cpu, get_r8(gb, z));
      break;

    case 7: /* CP r8 */
      CP_n8(&gb->cpu, get_r8(gb, z));
      break;
    }
  } break;

  case 3: /* Block 3 */ {
    switch (opcode & 0x7) {
    case 6: {
      switch (y) {
      case 0: /* ADD imm8 */
        ADD_n8(&gb->cpu, fetch_byte(gb));
        break;

      case 1: /* ADC imm8 */
        ADC_n8(&gb->cpu, fetch_byte(gb));
        break;

      case 2: /* SUB imm8 */
        SUB_n8(&gb->cpu, fetch_byte(gb));
        break;

      case 3: /* SBC imm8 */
        SBC_n8(&gb->cpu, fetch_byte(gb));
        break;

      case 4: /* AND imm8 */
        AND_n8(&gb->cpu, fetch_byte(gb));
        break;

      case 5: /* XOR imm8 */
        XOR_n8(&gb->cpu, fetch_byte(gb));
        break;

      case 6: /* OR imm8 */
        OR_n8(&gb->cpu, fetch_byte(gb));
        break;

      case 7: /* CP imm8 */
        CP_n8(&gb->cpu, fetch_byte(gb));
        break;
      }
    } break;

    case 7: /* RST tgt3 */ {
      push_onto_stack(gb, gb->cpu.PC);
      gb->cpu.PC = y * 8;
    } break;
    }
  } break;
  }
}

void execute_cb_instruction(GameBoy *gb, uint8_t opcode) {
  const uint8_t x = (opcode >> 6) & 0x3;
  const uint8_t y = (opcode >> 3) & 0x7;
  const uint8_t z = opcode & 0x7;

  switch (x) {
  case 0: /* Block 0: Bit shift operations */ {
    switch (y) {
    case 0: /* RLC r8 */
      set_r8(gb, z, RLC_r8(&gb->cpu, get_r8(gb, z)));
      break;

    case 1: /* RRC r8 */
      set_r8(gb, z, RRC_r8(&gb->cpu, get_r8(gb, z)));
      break;

    case 2: /* RL r8 */
      set_r8(gb, z, RL_r8(&gb->cpu, get_r8(gb, z)));
      break;

    case 3: /* RR r8 */
      set_r8(gb, z, RR_r8(&gb->cpu, get_r8(gb, z)));
      break;

    case 4: /* SLA r8 */
      set_r8(gb, z, SLA_r8(&gb->cpu, get_r8(gb, z)));
      break;

    case 5: /* SRA r8 */
      set_r8(gb, z, SRA_r8(&gb->cpu, get_r8(gb, z)));
      break;

    case 6: /* SWAP r8 */
      set_r8(gb, z, SWAP_r8(&gb->cpu, get_r8(gb, z)));
      break;

    case 7: /* SRL r8 */
      set_r8(gb, z, SRL_r8(&gb->cpu, get_r8(gb, z)));
      break;
    }
  } break;

  case 1: /* Block 1: BIT b3 r8 */ {
    BIT_b3_r8(&gb->cpu, y, get_r8(gb, z));
  } break;

  case 2: /* Block 2: RES b3 r8 */ {
    set_r8(gb, z, get_r8(gb, z) & ~(1 << y));
  } break;

  case 3: /* Block 3: SET b3 r8 */ {
    set_r8(gb, z, get_r8(gb, z) | (1 << y));
  } break;
  }
}

/*
//
// Jumps and subroutine instructions
//

void call_a16(GameBoy *gameboy) {
  const uint16_t jmp_addr = fetch_word(gameboy);
  push_stack(gameboy, gameboy->cpu.PC);
  gameboy->cpu.PC = jmp_addr;
}

void call_cc_a16(GameBoy *gameboy, bool cond) {
  const uint16_t jmp_addr = fetch_word(gameboy);
  if (cond) {
    push_stack(gameboy, gameboy->cpu.PC);
    gameboy->cpu.PC = jmp_addr;
    gameboy->cycles += 12;
  }
}

void jp_hl(CPU *cpu) { cpu->PC = get_r8_pair(cpu, REG_HL); }

void jp_a16(CPU *cpu) { cpu->PC = fetch_word(cpu); }

void jp_cc_a16(CPU *cpu) {
  const uint16_t jmp_addr = fetch_word(cpu);
  if (check_condition(cpu, (Condition)(cpu->y_field & 0x3))) {
    cpu->PC = jmp_addr;
    cpu->gameboy->cycles += 4;
  }
}

void jr_e8(CPU *cpu) { cpu->PC += (int8_t)fetch_byte(cpu); }

void jr_cc_e8(CPU *cpu) {
}

void ret(CPU *cpu) { cpu->PC = pop_stack(cpu); }

void ret_cc(CPU *cpu) {
  if (check_condition(cpu, (Condition)(cpu->y_field & 0x3))) {
    cpu->PC = pop_stack(cpu);
    cpu->gameboy->cycles += 12;
  }
}

void reti(CPU *cpu) {
  cpu->PC = pop_stack(cpu);
  cpu->IME = true;
}

void rst_vec(CPU *cpu) {
}

//
// Carry flag instructions
//

void ccf(CPU *cpu) {
}

void scf(CPU *cpu) {
}

//
// Stack manipulation instructions
//

uint16_t ADD_SP_e8(CPU *cpu) {
  const uint16_t SP = cpu->SP;
  const uint8_t n8 = fetch_byte(cpu);
  const int32_t sum = SP + (int8_t)n8;

  set_z(cpu, false);
  set_n(cpu, false);
  set_h(cpu, (SP & 0xF) + (n8 & 0xF) > 0xF);
  set_c(cpu, sum > 0xFF);

  return sum;
}

void add_sp_e8(CPU *cpu) { cpu->SP = ADD_SP_e8(cpu); }

void ld_mem_n16_sp(CPU *cpu) {
  const uint16_t addr = fetch_word(cpu);
  write_byte(cpu->gameboy, addr, cpu->SP & 0xFF);
  write_byte(cpu->gameboy, addr + 1, cpu->SP >> 8);
}

void ld_hl_sp_e8(CPU *cpu) { set_r8_pair(cpu, REG_HL, ADD_SP_e8(cpu)); }

void ld_sp_hl(CPU *cpu) { cpu->SP = get_r8_pair(cpu, REG_HL); }

void pop_r16(CPU *cpu) { set_r16stk(cpu, pop_stack(cpu)); }

void push_r16(CPU *cpu) { push_stack(cpu, get_r16stk(cpu)); }

//
// Interrupt-related instructions
//

void di(CPU *cpu) { cpu->IME = false; }

void ei(CPU *cpu) { cpu->IME = true; }

void halt(CPU *cpu) {
  // TODO: Proper halt behavior, halt bug.
  cpu->gameboy->state = GB_HALTED;
}

//
// Misc.
//

void prefix(CPU *cpu) {
  const uint8_t opcode = fetch_byte(cpu);
  cpu->y_field = (opcode >> 3) & 0x7;
  cpu->z_field = opcode & 0x7;

  Instruction cb_instruction = cb_optable[opcode];
  cpu->gameboy->cycles = cb_instruction.cycles;
  cb_instruction.exec(cpu);
}

void nop(CPU *cpu) {};

void stop_n8(CPU *cpu) {
  (void)fetch_byte(cpu);
  cpu->gameboy->state = GB_STOPPED;
}

void illegal(CPU *cpu) {
  fprintf(stderr, "Illegal opcode encountered");
  cpu->gameboy->state = GB_STOPPED;
}
*/
