#include "dmgb.h"
#include "bus.h"
#include "instructions.h"
#include "mnemonics.h"
#include "register_file.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define SUPPRESS_LOGS 0

static uint8_t dmgb_get_r8(struct dmgb *dmgb, uint8_t op_field);
static void dmgb_set_r8(struct dmgb *dmgb, uint8_t op_field, uint8_t u8);

static void dmgb_execute(struct dmgb *dmgb, uint8_t opcode);
static void dmgb_execute_cb(struct dmgb *dmgb, uint8_t opcode);

static void dmgb_log_state(const struct dmgb *dmgb, uint8_t opcode,
                           const char *mnemonic);

bool dmgb_create(struct dmgb *dmgb, const char *rom_path) {
  dmgb_destroy(dmgb);
  *dmgb = (struct dmgb){0};

  dmgb->reg.PC = 0x0100; // skip the bootrom
  dmgb->joypad.p1 = 0xFF;

  return cartridge_create(&dmgb->cartridge, rom_path);
}

void dmgb_destroy(struct dmgb *dmgb) { cartridge_destroy(&dmgb->cartridge); }

void dmgb_step(struct dmgb *dmgb) {
  // todo
  if (dmgb->ime_pending) {
    dmgb->ime_pending = false;
    dmgb->reg.IME = true;
  }

  switch (dmgb->state) {
  case DMGB_RUNNING: dmgb_execute(dmgb, dmgb_fetch_u8(dmgb)); break;
  case DMGB_HALTED: dmgb_tick(dmgb); break;
  case DMGB_HALT_BUG:
    dmgb->state = DMGB_RUNNING;
    dmgb_execute(dmgb, dmgb_read_u8(dmgb, dmgb->reg.PC));
    break;
  case DMGB_STOPPED: fprintf(stderr, "STOP instr\n"); break;
  }

  const uint8_t pending = dmgb->IE & dmgb->IF;
  if (pending > 0) {
    dmgb->state = DMGB_RUNNING;
    if (dmgb->reg.IME) {
      dmgb->reg.IME = false;
      for (uint8_t i = 0; i < 5; ++i) {
        if (pending >> i & 1u) {
          // Two wait states are executed before calling the interrupt handler
          dmgb_tick(dmgb);
          dmgb_tick(dmgb);
          dmgb_call(dmgb, 0x40u | i << 3u, true);
          dmgb->IF ^= 1u << i; // Interrupt handled
          break;
        }
      }
    }
  }
}

void dmgb_tick(struct dmgb *dmgb) {
  // ppu_tick(&dmgb->ppu);
  timer_tick(&dmgb->timer, &dmgb->IF);
}

uint8_t dmgb_get_r8(struct dmgb *dmgb, uint8_t opcode_field) {
  assert(opcode_field <= 0x7);
  switch (opcode_field) {
  case 0: return dmgb->reg.B;
  case 1: return dmgb->reg.C;
  case 2: return dmgb->reg.D;
  case 3: return dmgb->reg.E;
  case 4: return dmgb->reg.H;
  case 5: return dmgb->reg.L;
  case 6: return dmgb_read_u8(dmgb, rf_get_hl(&dmgb->reg));
  case 7: return dmgb->reg.A;
  default: assert(false);
  }
}

void dmgb_set_r8(struct dmgb *dmgb, uint8_t opcode_field, uint8_t u8) {
  assert(opcode_field <= 0x7);
  switch (opcode_field) {
  case 0: dmgb->reg.B = u8; break;
  case 1: dmgb->reg.C = u8; break;
  case 2: dmgb->reg.D = u8; break;
  case 3: dmgb->reg.E = u8; break;
  case 4: dmgb->reg.H = u8; break;
  case 5: dmgb->reg.L = u8; break;
  case 6: dmgb_write_u8(dmgb, rf_get_hl(&dmgb->reg), u8); break;
  case 7: dmgb->reg.A = u8; break;
  default: assert(false);
  }
}

void dmgb_execute(struct dmgb *dmgb, uint8_t opcode) {
  const uint8_t x = opcode >> 6u & 0x3u, y = opcode >> 3u & 0x7u,
                z = opcode & 0x7u;
  switch (x) {
  case 0: {
    switch (z) {
      // clang-format off
    case 4: dmgb_set_r8(dmgb, y, dmgb_inc_u8(dmgb, dmgb_get_r8(dmgb, y))); break;
    case 5: dmgb_set_r8(dmgb, y, dmgb_dec_u8(dmgb, dmgb_get_r8(dmgb, y))); break;
    case 6: dmgb_set_r8(dmgb, y, dmgb_fetch_u8(dmgb)); break;
    default: {
      const uint8_t A = dmgb->reg.A, F = dmgb->reg.F;
      switch (opcode) {
      case 0x00: /* NOP */ break;
      case 0x01: rf_set_bc(&dmgb->reg, dmgb_fetch_u16(dmgb)); break;
      case 0x02: dmgb_write_u8(dmgb, rf_get_bc(&dmgb->reg), A); break;
      case 0x03: rf_inc_bc(&dmgb->reg); dmgb_tick(dmgb); break;
      case 0x07: dmgb->reg.A = dmgb_rlc_u8(dmgb, A); dmgb->reg.F &= ~FLAG_Z; break;
      case 0x08: dmgb_write_u16(dmgb, dmgb_fetch_u16(dmgb), dmgb->reg.SP); break;
      case 0x09: dmgb_add_r16(dmgb, rf_get_bc(&dmgb->reg)); break;
      case 0x0A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_get_bc(&dmgb->reg)); break;
      case 0x0B: rf_dec_bc(&dmgb->reg); dmgb_tick(dmgb); break;
      case 0x0F: dmgb->reg.A = dmgb_rrc_u8(dmgb, A); dmgb->reg.F &= ~FLAG_Z; break;
      case 0x10: dmgb->state = DMGB_STOPPED; break;
      case 0x11: rf_set_de(&dmgb->reg, dmgb_fetch_u16(dmgb)); break;
      case 0x12: dmgb_write_u8(dmgb, rf_get_de(&dmgb->reg), A); break;
      case 0x13: rf_inc_de(&dmgb->reg); dmgb_tick(dmgb); break;
      case 0x17: dmgb->reg.A = dmgb_rl_u8(dmgb, A); dmgb->reg.F &= ~FLAG_Z; break;
      case 0x18: dmgb_jump_rotation(dmgb, dmgb_fetch_s8(dmgb), true); break;
      case 0x19: dmgb_add_r16(dmgb, rf_get_de(&dmgb->reg)); break;
      case 0x1A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_get_de(&dmgb->reg)); break;
      case 0x1B: rf_dec_de(&dmgb->reg); dmgb_tick(dmgb); break;
      case 0x1F: dmgb->reg.A = dmgb_rr_u8(dmgb, A); dmgb->reg.F &= ~FLAG_Z; break;
      case 0x20: dmgb_jump_rotation(dmgb, dmgb_fetch_s8(dmgb), (F & FLAG_Z) == 0); break;
      case 0x21: rf_set_hl(&dmgb->reg, dmgb_fetch_u16(dmgb)); break;
      case 0x22: dmgb_write_u8(dmgb, rf_inc_hl(&dmgb->reg), A); break;
      case 0x23: rf_inc_hl(&dmgb->reg); dmgb_tick(dmgb); break;
      case 0x27: dmgb_daa(dmgb); break;
      case 0x28: dmgb_jump_rotation(dmgb, dmgb_fetch_s8(dmgb), (F & FLAG_Z) != 0); break;
      case 0x29: dmgb_add_r16(dmgb, rf_get_hl(&dmgb->reg)); break;
      case 0x2A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_inc_hl(&dmgb->reg)); break;
      case 0x2B: rf_dec_hl(&dmgb->reg); dmgb_tick(dmgb); break;
      case 0x2F: dmgb->reg.A = ~A; dmgb->reg.F |= FLAG_N | FLAG_H; break;
      case 0x30: dmgb_jump_rotation(dmgb, dmgb_fetch_s8(dmgb), (F & FLAG_C) == 0); break;
      case 0x31: dmgb->reg.SP = dmgb_fetch_u16(dmgb); break;
      case 0x32: dmgb_write_u8(dmgb, rf_dec_hl(&dmgb->reg), A); break;
      case 0x33: dmgb->reg.SP++; dmgb_tick(dmgb); break;
      case 0x37: dmgb->reg.F &= ~(FLAG_N | FLAG_H); dmgb->reg.F |= FLAG_C; break;
      case 0x38: dmgb_jump_rotation(dmgb, dmgb_fetch_s8(dmgb), (F & FLAG_C) != 0); break;
      case 0x39: dmgb_add_r16(dmgb, dmgb->reg.SP); break;
      case 0x3A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_dec_hl(&dmgb->reg)); break;
      case 0x3B: dmgb->reg.SP--; dmgb_tick(dmgb); break;
      case 0x3F: dmgb->reg.F &= ~(FLAG_N | FLAG_H); dmgb->reg.F ^= FLAG_C; break;
      default: assert(false);
      }
    } break;
    }
    // clang-format on
  } break;

  case 1: {
    if (opcode == 0x76) {
      // HALT
      if (!dmgb->reg.IME && (dmgb->IE & dmgb->IF) > 0)
        dmgb->state = DMGB_HALT_BUG;
      else
        dmgb->state = DMGB_HALTED;
    } else if (y != z) {
      dmgb_set_r8(dmgb, y, dmgb_get_r8(dmgb, z));
    }
  } break;

  case 2: {
    const uint8_t r8 = dmgb_get_r8(dmgb, z);
    switch (y) {
    case 0: dmgb_add_u8(dmgb, r8); break;
    case 1: dmgb_adc_u8(dmgb, r8); break;
    case 2: dmgb_sub_u8(dmgb, r8); break;
    case 3: dmgb_sbc_u8(dmgb, r8); break;
    case 4: dmgb_and_u8(dmgb, r8); break;
    case 5: dmgb_xor_u8(dmgb, r8); break;
    case 6: dmgb_or_u8(dmgb, r8); break;
    case 7: dmgb_cp_u8(dmgb, r8); break;
    default: assert(false);
    }
  } break;

  case 3: {
    switch (z) {
    case 6: {
      const uint8_t imm8 = dmgb_fetch_u8(dmgb);
      switch (y) {
      case 0: dmgb_add_u8(dmgb, imm8); break;
      case 1: dmgb_adc_u8(dmgb, imm8); break;
      case 2: dmgb_sub_u8(dmgb, imm8); break;
      case 3: dmgb_sbc_u8(dmgb, imm8); break;
      case 4: dmgb_and_u8(dmgb, imm8); break;
      case 5: dmgb_xor_u8(dmgb, imm8); break;
      case 6: dmgb_or_u8(dmgb, imm8); break;
      case 7: dmgb_cp_u8(dmgb, imm8); break;
      default: assert(false);
      }
    } break;
    case 7: {
      dmgb_call(dmgb, y * 8, true); // RST
    } break;
    default: {
      const uint8_t A = dmgb->reg.A, C = dmgb->reg.C, F = dmgb->reg.F;
      switch (opcode) {
        // clang-format off
      case 0xC0: dmgb_tick(dmgb); dmgb_return(dmgb, (F & FLAG_Z) == 0); break;
      case 0xC1: rf_set_bc(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
      case 0xC2: dmgb_jump(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_Z) == 0); break;
      case 0xC3: dmgb_jump(dmgb, dmgb_fetch_u16(dmgb), true); break;
      case 0xC4: dmgb_call(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_Z) == 0); break;
      case 0xC5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_bc(&dmgb->reg)); break;
      case 0xC8: dmgb_tick(dmgb); dmgb_return(dmgb, (F & FLAG_Z) != 0); break;
      case 0xC9: dmgb_return(dmgb, true); break;
      case 0xCA: dmgb_jump(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_Z) != 0); break;
      case 0xCB: dmgb_execute_cb(dmgb, dmgb_fetch_u8(dmgb)); break;
      case 0xCC: dmgb_call(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_Z) != 0); break;
      case 0xCD: dmgb_call(dmgb, dmgb_fetch_u16(dmgb), true); break;
      case 0xD0: dmgb_tick(dmgb); dmgb_return(dmgb, (F & FLAG_C) == 0); break;
      case 0xD1: rf_set_de(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
      case 0xD2: dmgb_jump(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_C) == 0); break;
      case 0xD3: dmgb_illegal(dmgb, opcode); break;
      case 0xD4: dmgb_call(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_C) == 0); break;
      case 0xD5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_de(&dmgb->reg)); break;
      case 0xD8: dmgb_tick(dmgb); dmgb_return(dmgb, (F & FLAG_C) != 0); break;
      case 0xD9: dmgb_return(dmgb, dmgb->ime_pending = true); break;
      case 0xDA: dmgb_jump(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_C) != 0); break;
      case 0xDB: dmgb_illegal(dmgb, opcode); break;
      case 0xDC: dmgb_call(dmgb, dmgb_fetch_u16(dmgb), (F & FLAG_C) != 0); break;
      case 0xDD: dmgb_illegal(dmgb, opcode); break;
      case 0xE0: dmgb_write_u8(dmgb, 0xFF00 + dmgb_fetch_u8(dmgb), A); break;
      case 0xE1: rf_set_hl(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
      case 0xE2: dmgb_write_u8(dmgb, 0xFF00 + C, A); break;
      case 0xE3: dmgb_illegal(dmgb, opcode); break;
      case 0xE4: dmgb_illegal(dmgb, opcode); break;
      case 0xE5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_hl(&dmgb->reg)); break;
      case 0xE8: dmgb->reg.SP = dmgb_add_sp_i8(dmgb, dmgb_fetch_s8(dmgb)); dmgb_tick(dmgb); break;
      case 0xE9: dmgb->reg.PC = rf_get_hl(&dmgb->reg); break;
      case 0xEA: dmgb_write_u8(dmgb, dmgb_fetch_u16(dmgb), A); break;
      case 0xEB: dmgb_illegal(dmgb, opcode); break;
      case 0xEC: dmgb_illegal(dmgb, opcode); break;
      case 0xED: dmgb_illegal(dmgb, opcode); break;
      case 0xF0: dmgb->reg.A = dmgb_read_u8(dmgb, 0xFF00 + dmgb_fetch_u8(dmgb)); break;
      case 0xF1: rf_set_af(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
      case 0xF2: dmgb->reg.A = dmgb_read_u8(dmgb, 0xFF00 + C); break;
      case 0xF3: dmgb->reg.IME = dmgb->ime_pending = false; break;
      case 0xF4: dmgb_illegal(dmgb, opcode); break;
      case 0xF5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_af(&dmgb->reg)); break;
      case 0xF8: rf_set_hl(&dmgb->reg, dmgb_add_sp_i8(dmgb, dmgb_fetch_s8(dmgb))); break;
      case 0xF9: dmgb->reg.SP = rf_get_hl(&dmgb->reg); dmgb_tick(dmgb); break;
      case 0xFA: dmgb->reg.A = dmgb_read_u8(dmgb, dmgb_fetch_u16(dmgb)); break;
      case 0xFB: dmgb->ime_pending = true; break;
      case 0xFC: dmgb_illegal(dmgb, opcode); break;
      case 0xFD: dmgb_illegal(dmgb, opcode); break;
      default: assert(false);
        // clang-format on
      }
    } break;
    }
  } break;

  default: assert(false);
  }

#if !defined(NDEBUG) && !SUPPRESS_LOGS
  dmgb_log_state(dmgb, opcode, mnemonics[opcode]);
#endif
}

void dmgb_execute_cb(struct dmgb *dmgb, uint8_t opcode) {
  const uint8_t x = opcode >> 6u & 0x3u, y = opcode >> 3u & 0x7u,
                z = opcode & 0x7u;
  const uint8_t r8 = dmgb_get_r8(dmgb, z);
  switch (x) {
  case 0:
    switch (y) {
    case 0: dmgb_set_r8(dmgb, z, dmgb_rlc_u8(dmgb, r8)); break;
    case 1: dmgb_set_r8(dmgb, z, dmgb_rrc_u8(dmgb, r8)); break;
    case 2: dmgb_set_r8(dmgb, z, dmgb_rl_u8(dmgb, r8)); break;
    case 3: dmgb_set_r8(dmgb, z, dmgb_rr_u8(dmgb, r8)); break;
    case 4: dmgb_set_r8(dmgb, z, dmgb_sla_u8(dmgb, r8)); break;
    case 5: dmgb_set_r8(dmgb, z, dmgb_sra_u8(dmgb, r8)); break;
    case 6: dmgb_set_r8(dmgb, z, dmgb_swap_u8(dmgb, r8)); break;
    case 7: dmgb_set_r8(dmgb, z, dmgb_srl_u8(dmgb, r8)); break;
    default: assert(false);
    }
    break;
  case 1: dmgb_bit_b3_u8(dmgb, y, r8); break;
  case 2: dmgb_set_r8(dmgb, z, r8 & ~(1u << y)); break;
  case 3: dmgb_set_r8(dmgb, z, r8 | 1u << y); break;
  default: assert(false);
  }

#if !defined(NDEBUG) && !SUPPRESS_LOGS
  dmgb_log_state(dmgb, opcode, mnemonics_cb[opcode]);
#endif
}

void dmgb_log_state(const struct dmgb *dmgb, uint8_t opcode,
                    const char *mnemonic) {
  const uint16_t AF = rf_get_af(&dmgb->reg), BC = rf_get_bc(&dmgb->reg),
                 DE = rf_get_de(&dmgb->reg), HL = rf_get_hl(&dmgb->reg),
                 SP = dmgb->reg.SP, PC = dmgb->reg.PC;
  printf("\n"
         "%02X: %s\n"
         "AF:%04X BC:%04X DE:%04X HL:%04X SP:%04X PC:%04X [BC]:%02X [DE]:%02X "
         "[HL]:%02X [SP]:%02X [PC]:%02X,%02X,%02X,%02X\n"
         "\n",
         opcode, mnemonic, AF, BC, DE, HL, SP, PC, dmgb_bus_read(dmgb, BC),
         dmgb_bus_read(dmgb, DE), dmgb_bus_read(dmgb, HL),
         dmgb_bus_read(dmgb, SP), dmgb_bus_read(dmgb, PC),
         dmgb_bus_read(dmgb, PC + 1), dmgb_bus_read(dmgb, PC + 2),
         dmgb_bus_read(dmgb, PC + 3));
}
