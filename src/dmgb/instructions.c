#include "instructions.h"
#include "bus.h"
#include "dmgb.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

static uint8_t dmgb_set_flags(bool z, bool n, bool h, bool c) {
  return z << 7u | n << 6u | h << 5u | c << 4u;
}

/// 8-bit arithmetic implementations

void dmgb_add_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t A = dmgb->reg.A;

  const uint16_t sum = A + u8;
  const uint8_t result = (uint8_t)sum;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, (A & 0xF) + (u8 & 0xF) > 0xF,
                               sum > 0xFF);

  dmgb->reg.A = result;
}

void dmgb_adc_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t A = dmgb->reg.A;
  const bool carry = dmgb->reg.F & FLAG_C;

  const uint16_t sum = A + u8 + carry;
  const uint8_t result = (uint8_t)sum;

  dmgb->reg.F = dmgb_set_flags(
      result == 0, false, (A & 0xF) + (u8 & 0xF) + carry > 0xF, sum > 0xFF);

  dmgb->reg.A = result;
}

void dmgb_sub_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t A = dmgb->reg.A;
  const uint8_t result = A - u8;

  dmgb->reg.F =
      dmgb_set_flags(result == 0, true, (A & 0xF) < (u8 & 0xF), A < u8);

  dmgb->reg.A = result;
}

void dmgb_sbc_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t A = dmgb->reg.A;
  const bool carry = dmgb->reg.F & FLAG_C;
  const uint8_t result = A - (u8 + carry);

  dmgb->reg.F =
      dmgb_set_flags(result == 0, true, (A & 0xF) < (u8 & 0xF) + carry,
                     A < (uint16_t)(u8 + carry));

  dmgb->reg.A = result;
}

void dmgb_cp_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t A = dmgb->reg.A;

  dmgb->reg.F =
      dmgb_set_flags(A - u8 == 0, true, (A & 0xF) < (u8 & 0xF), A < u8);
}

uint8_t dmgb_inc_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t result = u8 + 1;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, (u8 & 0xF) == 0xF,
                               dmgb->reg.F & FLAG_C);

  return result;
}

uint8_t dmgb_dec_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t result = u8 - 1;

  dmgb->reg.F =
      dmgb_set_flags(result == 0, true, (u8 & 0xF) == 0, dmgb->reg.F & FLAG_C);

  return result;
}

void dmgb_add_r16(struct dmgb *dmgb, uint16_t u16) {
  const uint16_t HL = rf_get_hl(&dmgb->reg);
  const uint32_t sum = HL + u16;

  dmgb->reg.F =
      dmgb_set_flags(dmgb->reg.F & FLAG_Z, false,
                     (HL & 0xFFF) + (u16 & 0xFFF) > 0xFFF, sum > 0xFFFF);

  rf_set_hl(&dmgb->reg, sum);
}

uint16_t dmgb_add_sp_i8(struct dmgb *dmgb, int8_t i8) {
  const uint16_t SP = dmgb->reg.SP;
  const uint16_t sum = SP + i8;

  dmgb->reg.F = dmgb_set_flags(false, false, (SP & 0xF) + (i8 & 0xF) > 0xF,
                               (SP & 0xFF) + (uint8_t)i8 > 0xFF);

  return sum;
}

/// Bitwise logic implementations

void dmgb_and_u8(struct dmgb *dmgb, uint8_t u8) {
  dmgb->reg.F = dmgb_set_flags((dmgb->reg.A &= u8) == 0, false, true, false);
}

void dmgb_xor_u8(struct dmgb *dmgb, uint8_t u8) {
  dmgb->reg.F = dmgb_set_flags((dmgb->reg.A ^= u8) == 0, false, false, false);
}

void dmgb_or_u8(struct dmgb *dmgb, uint8_t u8) {
  dmgb->reg.F = dmgb_set_flags((dmgb->reg.A |= u8) == 0, false, false, false);
}

/// Bit flag implementations

void dmgb_bit_b3_u8(struct dmgb *dmgb, uint8_t b3, uint8_t u8) {
  dmgb->reg.F =
      dmgb_set_flags((u8 >> b3 & 1) == 0, false, true, dmgb->reg.F & FLAG_C);
}

/// Bit-shift implementations

uint8_t dmgb_rl_u8(struct dmgb *dmgb, uint8_t u8) {
  const bool carry = dmgb->reg.F & FLAG_C;
  const uint8_t result = u8 << 1 | carry;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, (u8 & 0x80) != 0);

  return result;
}

uint8_t dmgb_rlc_u8(struct dmgb *dmgb, uint8_t u8) {
  const bool b7 = u8 >> 7 & 1;
  const uint8_t result = u8 << 1 | b7;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, b7);

  return result;
}

uint8_t dmgb_rr_u8(struct dmgb *dmgb, uint8_t u8) {
  const bool carry = dmgb->reg.F & FLAG_C;
  const uint8_t result = carry << 7 | u8 >> 1;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, u8 & 1);

  return result;
}

uint8_t dmgb_rrc_u8(struct dmgb *dmgb, uint8_t u8) {
  const bool b0 = u8 & 1;
  const uint8_t result = b0 << 7 | u8 >> 1;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, b0);

  return result;
}

uint8_t dmgb_sla_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t result = u8 << 1;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, u8 & 0x80);

  return result;
}

uint8_t dmgb_sra_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t result = (u8 & 0x80) | u8 >> 1;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, u8 & 1);

  return result;
}

uint8_t dmgb_srl_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t result = u8 >> 1;

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, u8 & 1);

  return result;
}

uint8_t dmgb_swap_u8(struct dmgb *dmgb, uint8_t u8) {
  const uint8_t result = (u8 << 4 & 0xF0) | (u8 >> 4 & 0x0F);

  dmgb->reg.F = dmgb_set_flags(result == 0, false, false, false);

  return result;
}

void dmgb_daa(struct dmgb *dmgb) {
  const uint8_t A = dmgb->reg.A, F = dmgb->reg.F;
  uint8_t result = 0, adjustment = 0;
  bool carry = dmgb->reg.F & FLAG_C;

  if (F & FLAG_N) {
    if (F & FLAG_H)
      adjustment |= 0x6;
    if (F & FLAG_C)
      adjustment |= 0x60;

    result = A - adjustment;
  } else {
    if ((F & FLAG_H) != 0 || (A & 0xF) > 0x9)
      adjustment |= 0x6;
    if ((F & FLAG_C) != 0 || A > 0x99) {
      adjustment |= 0x60;
      carry = true;
    }

    result = A + adjustment;
  }

  dmgb->reg.F = dmgb_set_flags(result == 0, dmgb->reg.F & FLAG_N, false, carry);

  dmgb->reg.A = result;
}

void dmgb_halt(struct dmgb *dmgb) {
  dmgb->state = !dmgb->reg.IME && (dmgb->IE & dmgb->IF) != 0 ? DMGB_HALT_BUG
                                                             : DMGB_HALTED;
}

void dmgb_illegal(struct dmgb *dmgb, uint8_t opcode) {
  fprintf(stderr, "Illegal instruction: 0x%02X\n", opcode);
}
