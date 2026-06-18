#include "bitwise.h"
#include "cpu.h"
#include "gameboy.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

/**
 * Helper functions
 */

uint8_t fetch_byte(struct gameboy *gb) { return read_byte(gb, gb->cpu.PC++); }

uint16_t fetch_word(struct gameboy *gb) {
  const uint8_t lo = read_byte(gb, gb->cpu.PC++);
  const uint8_t hi = read_byte(gb, gb->cpu.PC++);
  return (uint16_t)hi << 8 | lo;
}

uint8_t read_hl(struct gameboy *gb) {
  return read_byte(gb, get_r8_pair(&gb->cpu, REG_HL));
}

void write_hl(struct gameboy *gb, uint8_t val) {
  write_byte(gb, get_r8_pair(&gb->cpu, REG_HL), val);
}

uint8_t get_r8(struct gameboy *gb, uint8_t opcode_field) {
  if (opcode_field == 6)
    return read_hl(gb);
  else
    return gb->cpu.r8[opcode_field];
}

void set_r8(struct gameboy *gb, uint8_t opcode_field, uint8_t val) {
  if (opcode_field == 6)
    write_hl(gb, val);
  else
    gb->cpu.r8[opcode_field] = val;
}

uint16_t get_r16(const struct cpu *cpu, uint8_t y_field) {
  const uint16_t src = y_field >> 1;
  if (src == 3)
    return cpu->SP;
  else
    return get_r8_pair(cpu, (enum r16_idx)src);
}

void set_r16(struct cpu *cpu, uint8_t y_field, uint16_t val) {
  const uint16_t dest = y_field >> 1;
  if (dest == 3)
    cpu->SP = val;
  else
    set_r8_pair(cpu, (enum r16_idx)dest, val);
}

uint16_t get_r16stk(const struct cpu *cpu, uint8_t y_field) {
  const uint16_t src = y_field >> 1;
  if (src == 3)
    return (uint16_t)cpu->r8[REG_A] << 8 | cpu->r8[REG_F];
  else
    return get_r8_pair(cpu, (enum r16_idx)src);
}

void set_r16stk(struct cpu *cpu, uint8_t y_field, uint16_t val) {
  const uint16_t dest = y_field >> 1;
  if (dest == 3) {
    cpu->r8[REG_A] = val >> 8;
    cpu->r8[REG_F] = val & 0xF0;
  } else {
    set_r8_pair(cpu, (enum r16_idx)dest, val);
  }
}

uint16_t get_r16mem(struct cpu *cpu, uint8_t y_field) {
  const uint8_t r16mem = y_field >> 1;
  if (r16mem == 0 || r16mem == 1) /* BC or DE */ {
    return get_r8_pair(cpu, (enum r16_idx)r16mem);
  } else {
    const uint16_t ret = get_r8_pair(cpu, REG_HL);
    if (r16mem == 2) /* HLI */
      set_r8_pair(cpu, REG_HL, ret + 1);
    else /* HLD */
      set_r8_pair(cpu, REG_HL, ret - 1);
    return ret;
  }
}

enum condition { COND_NZ = 0, COND_Z = 1, COND_NC = 2, COND_C = 3 };
bool check_condition(struct cpu *cpu, enum condition cond) {
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

void push_onto_stack(struct gameboy *gb, uint16_t val) {
  write_byte(gb, --gb->cpu.SP, val >> 8);
  write_byte(gb, --gb->cpu.SP, val & 0xFF);
}

uint16_t pop_off_stack(struct gameboy *gb) {
  const uint8_t lo = read_byte(gb, gb->cpu.SP++);
  const uint8_t hi = read_byte(gb, gb->cpu.SP++);
  return (uint16_t)hi << 8 | lo;
}

/**
 * 8-bit arithmetic instructions
 */

void ADD_n8(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint16_t sum = A + operand;
  const uint8_t result = (uint8_t)sum;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, (A & 0xF) + (operand & 0xF) > 0xF);
  set_c(cpu, sum > 0xFF);

  cpu->r8[REG_A] = result;
}

void ADC_n8(struct cpu *cpu, const uint8_t operand) {
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

void SUB_n8(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A - operand;

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF));
  set_c(cpu, A < operand);

  cpu->r8[REG_A] = result;
}

void SBC_n8(struct cpu *cpu, const uint8_t operand) {
  const uint8_t carry = is_c_set(cpu);
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A - (operand + carry);

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF) + carry);
  set_c(cpu, A < operand + carry);

  cpu->r8[REG_A] = result;
}

void CP_n8(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  set_z(cpu, A - operand == 0);
  set_n(cpu, true);
  set_h(cpu, (A & 0xF) < (operand & 0xF));
  set_c(cpu, A < operand);
}

uint8_t DEC_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand - 1;

  set_z(cpu, result == 0);
  set_n(cpu, true);
  set_h(cpu, (operand & 0xF) == 0x0);

  return result;
}

uint8_t INC_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand + 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, (operand & 0xF) == 0xF);

  return result;
}

/**
 * 16-bit arithmetic instructions
 */

void ADD_r16(struct cpu *cpu, uint16_t operand) {
  const uint16_t HL = get_r8_pair(cpu, REG_HL);
  const uint32_t sum = HL + operand;

  set_n(cpu, false);
  set_h(cpu, (HL & 0xFFF) + (operand & 0xFFF) > 0xFFF);
  set_c(cpu, sum > 0xFFFF);

  set_r8_pair(cpu, REG_HL, sum);
}

uint16_t ADD_SP_e8(struct gameboy *gb) {
  const uint16_t SP = gb->cpu.SP;
  const uint8_t n8 = fetch_byte(gb);
  const int32_t sum = SP + (int8_t)n8;

  set_z(&gb->cpu, false);
  set_n(&gb->cpu, false);
  set_h(&gb->cpu, (SP & 0xF) + (n8 & 0xF) > 0xF);
  set_c(&gb->cpu, sum > 0xFF);

  return sum;
}

/**
 * Bitwise logic instructions
 */

void AND_n8(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A & operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, true);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

void XOR_n8(struct cpu *cpu, const uint8_t operand) {
  const uint8_t A = cpu->r8[REG_A];

  const uint8_t result = A ^ operand;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, false);

  cpu->r8[REG_A] = result;
}

void OR_n8(struct cpu *cpu, const uint8_t operand) {
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

void BIT_b3_r8(struct cpu *cpu, uint8_t b3, uint8_t r8) {
  set_z(cpu, !get_bit(r8, b3));
  set_n(cpu, false);
  set_h(cpu, true);
}

/**
 * Bit shift instructions
 */

uint8_t RL_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | is_c_set(cpu);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  return result;
}

uint8_t RLC_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1 | is_c_set(cpu);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  return result;
}

uint8_t RR_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (is_c_set(cpu) << 7) | (operand >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t RRC_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = ((operand & 1) << 7) | (operand >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t SLA_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand << 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 0x80);

  return result;
}

uint8_t SRA_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = (operand & 0x80) | (operand >> 1);

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t SRL_r8(struct cpu *cpu, uint8_t operand) {
  const uint8_t result = operand >> 1;

  set_z(cpu, result == 0);
  set_n(cpu, false);
  set_h(cpu, false);
  set_c(cpu, operand & 1);

  return result;
}

uint8_t SWAP_r8(struct cpu *cpu, uint8_t operand) {
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

void DAA(struct cpu *cpu) {
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

void ILLEGAL(struct gameboy *gameboy, uint8_t opcode) {
  gameboy->state = GB_STOPPED;
  fprintf(stderr, "Illegal instruction: 0x%2X\n", opcode);
}

/**
 * Instruction decoding
 */

void execute_instruction(struct gameboy *gb, uint8_t opcode) {
  uint8_t x = (opcode >> 6) & 0x3;
  uint8_t y = (opcode >> 3) & 0x7;
  uint8_t z = opcode & 0x7;

  switch (x) {
  case 0: /* Block 0 */ {
    switch (z) {
    case 0: {
      if (y == 0) /* NOP */ {
        return;
      } else if (y == 1) /* LD [a16] SP */ {
        write_byte(gb, fetch_word(gb), gb->cpu.SP);
      } else if (y == 2) /* STOP */ {
        gb->state = GB_STOPPED;
      } else if (y == 3) /* JR e8 */ {
        gb->cpu.PC += (int8_t)fetch_byte(gb);
      } else if (y > 3) /* JR cc, e8 */ {
        const int8_t offset = fetch_byte(gb);
        if (check_condition(&gb->cpu, (enum condition)(y & 0x3))) {
          gb->cpu.PC += offset;
          gb->cycles += 4;
        }
      }
    } break;

    case 1: {
      if ((y & 1) == 0) /* LD r16, imm16 */
        set_r16(&gb->cpu, y, fetch_word(gb));
      else /* ADD HL, r16 */
        ADD_r16(&gb->cpu, get_r16(&gb->cpu, y));
    } break;

    case 2: {
      if ((y & 1) == 0) /* LD [r16mem], A */
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
    switch (z) {
    case 0: {
      if (y < 4) /* RET cc */ {
        if (check_condition(&gb->cpu, (enum condition)(y & 0x3))) {
          gb->cpu.PC = pop_off_stack(gb);
          gb->cycles += 12;
        }
      } else if (y == 4) /* LDH [a8], A */ {
        write_byte(gb, 0xFF00 | fetch_byte(gb), gb->cpu.r8[REG_A]);
      } else if (y == 5) /* ADD SP, e8 */ {
        gb->cpu.SP = ADD_SP_e8(gb);
      } else if (y == 6) /* LDH A, [a8] */ {
        gb->cpu.r8[REG_A] = read_byte(gb, 0xFF00 | fetch_byte(gb));
      } else if (y == 7) /* LD HL, SP + e8 */ {
        set_r8_pair(&gb->cpu, REG_HL, ADD_SP_e8(gb));
      }
    } break;

    case 1: {
      if ((y & 1) == 0) /* POP r16stk */ {
        set_r16stk(&gb->cpu, y, pop_off_stack(gb));
      } else if (y == 1) /* RET */ {
        gb->cpu.PC = pop_off_stack(gb);
      } else if (y == 3) /* RETI */ {
        gb->cpu.PC = pop_off_stack(gb);
        gb->cpu.IME = true;
      } else if (y == 5) /* JP HL */ {
        gb->cpu.PC = get_r8_pair(&gb->cpu, REG_HL);
      } else if (y == 7) /* LD SP, HL */ {
        gb->cpu.SP = get_r8_pair(&gb->cpu, REG_HL);
      }
    } break;

    case 2: {
      if (y < 4) /* JP cc, a16 */ {
        const uint16_t jmp_addr = fetch_word(gb);
        if (check_condition(&gb->cpu, (enum condition)(y & 0x3))) {
          gb->cpu.PC = jmp_addr;
          gb->cycles += 4;
        }
      } else if (y == 4) /* LDH [C], A */ {
        write_byte(gb, 0xFF00 | gb->cpu.r8[REG_C], gb->cpu.r8[REG_A]);
      } else if (y == 5) /* LD [a16], A */ {
        write_byte(gb, fetch_word(gb), gb->cpu.r8[REG_A]);
      } else if (y == 6) /* LDH A, [C] */ {
        gb->cpu.r8[REG_A] = read_byte(gb, 0xFF00 | gb->cpu.r8[REG_C]);
      } else if (y == 7) /* LD A, [a16]*/ {
        gb->cpu.r8[REG_A] = read_byte(gb, fetch_word(gb));
      }
    }

    case 3: {
      if (y == 0) /* JP a16 */ {
        gb->cpu.PC = fetch_word(gb);
      } else if (y == 1) /* PREFIX */ {
        execute_cb_instruction(gb, fetch_byte(gb));
      } else if (y == 6) /* DI */ {
        gb->cpu.IME = false;
      } else if (y == 7) /* EI*/ {
        gb->cpu.IME = true;
      } else {
        ILLEGAL(gb, opcode);
      }
    } break;

    case 4: {
      if (y < 4) /* CALL cc a16 */ {
        const uint16_t jmp_addr = fetch_word(gb);
        if (check_condition(&gb->cpu, (enum condition)(y & 0x3))) {
          push_onto_stack(gb, gb->cpu.PC);
          gb->cpu.PC = jmp_addr;
          gb->cycles += 12;
        }
      } else {
        ILLEGAL(gb, opcode);
      }
    } break;

    case 5: {
      if ((y & 1) == 0) /* PUSH r16stk */ {
        push_onto_stack(gb, get_r16stk(&gb->cpu, y));
      } else if (y == 1) /* CALL a16 */ {
        const uint16_t jmp_addr = fetch_word(gb);
        push_onto_stack(gb, gb->cpu.PC);
        gb->cpu.PC = jmp_addr;
      } else {
        ILLEGAL(gb, opcode);
      }
    } break;

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

void execute_cb_instruction(struct gameboy *gb, uint8_t opcode) {
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
