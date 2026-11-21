#include "dmgb.h"
#include "dmgb/bus.h"
#include "dmgb/instructions.h"
#include "dmgb/mnemonics.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SUPPRESS_LOGS 0

static void dmgb_log_state(const struct dmgb *dmgb, uint8_t opcode,
                           const char *mnemonic);

bool dmgb_create(struct dmgb *dmgb, const char *rom_path) {
  memset(dmgb, 0, sizeof(struct dmgb));

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
  case DMGB_RUNNING:
    dmgb_execute(dmgb, dmgb_read_imm8(dmgb));
    break;
  case DMGB_HALTED:
    dmgb_tick(dmgb);
    break;
  case DMGB_HALT_BUG:
    dmgb->state = DMGB_RUNNING;
    dmgb_execute(dmgb, dmgb_read_u8(dmgb, dmgb->reg.PC));
    break;
  case DMGB_STOPPED:
    fprintf(stderr, "STOP instr\n");
    break;
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

void dmgb_execute(struct dmgb *dmgb, uint8_t opcode) {
  switch (opcode) {
    // clang-format off
  case 0x00: /* NOP */ break;
  case 0x01: rf_set_bc(&dmgb->reg, dmgb_read_imm16(dmgb)); break;
  case 0x02: dmgb_write_u8(dmgb, rf_get_bc(&dmgb->reg), dmgb->reg.A); break;
  case 0x03: rf_inc_bc(&dmgb->reg); dmgb_tick(dmgb); break;
  case 0x04: dmgb->reg.B = dmgb_inc_u8(dmgb, dmgb->reg.B); break;
  case 0x05: dmgb->reg.B = dmgb_dec_u8(dmgb, dmgb->reg.B); break;
  case 0x06: dmgb->reg.B = dmgb_read_imm8(dmgb); break;
  case 0x07: dmgb->reg.A = dmgb_rlc_u8(dmgb, dmgb->reg.A); dmgb->reg.F &= ~FLAG_Z; break;
  case 0x08: dmgb_write_u16(dmgb, dmgb_read_imm16(dmgb), dmgb->reg.SP); break;
  case 0x09: dmgb_add_r16(dmgb, rf_get_bc(&dmgb->reg)); break;
  case 0x0A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_get_bc(&dmgb->reg)); break;
  case 0x0B: rf_dec_bc(&dmgb->reg); dmgb_tick(dmgb); break;
  case 0x0C: dmgb->reg.C = dmgb_inc_u8(dmgb, dmgb->reg.C); break;
  case 0x0D: dmgb->reg.C = dmgb_dec_u8(dmgb, dmgb->reg.C); break;
  case 0x0E: dmgb->reg.C = dmgb_read_imm8(dmgb); break;
  case 0x0F: dmgb->reg.A = dmgb_rrc_u8(dmgb, dmgb->reg.A); dmgb->reg.F &= ~FLAG_Z; break;
  case 0x10: dmgb->state = DMGB_STOPPED; break; // WARN: unimplemented
  case 0x11: rf_set_de(&dmgb->reg, dmgb_read_imm16(dmgb)); break;
  case 0x12: dmgb_write_u8(dmgb, rf_get_de(&dmgb->reg), dmgb->reg.A); break;
  case 0x13: rf_inc_de(&dmgb->reg); dmgb_tick(dmgb); break;
  case 0x14: dmgb->reg.D = dmgb_inc_u8(dmgb, dmgb->reg.D); break;
  case 0x15: dmgb->reg.D = dmgb_dec_u8(dmgb, dmgb->reg.D); break;
  case 0x16: dmgb->reg.D = dmgb_read_imm8(dmgb); break;
  case 0x17: dmgb->reg.A = dmgb_rl_u8(dmgb, dmgb->reg.A); dmgb->reg.F &= ~FLAG_Z; break;
  case 0x18: dmgb_jump_rotation(dmgb, (int8_t)dmgb_read_imm8(dmgb), true); break;
  case 0x19: dmgb_add_r16(dmgb, rf_get_de(&dmgb->reg)); break;
  case 0x1A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_get_de(&dmgb->reg)); break;
  case 0x1B: rf_dec_de(&dmgb->reg); dmgb_tick(dmgb); break;
  case 0x1C: dmgb->reg.E = dmgb_inc_u8(dmgb, dmgb->reg.E); break;
  case 0x1D: dmgb->reg.E = dmgb_dec_u8(dmgb, dmgb->reg.E); break;
  case 0x1E: dmgb->reg.E = dmgb_read_imm8(dmgb); break;
  case 0x1F: dmgb->reg.A = dmgb_rr_u8(dmgb, dmgb->reg.A); dmgb->reg.F &= ~FLAG_Z; break;
  case 0x20: dmgb_jump_rotation(dmgb, (int8_t)dmgb_read_imm8(dmgb), (dmgb->reg.F & FLAG_Z) == 0);  break;
  case 0x21: rf_set_hl(&dmgb->reg, dmgb_read_imm16(dmgb)); break;
  case 0x22: dmgb_write_u8(dmgb, rf_inc_hl(&dmgb->reg), dmgb->reg.A); break;
  case 0x23: rf_inc_hl(&dmgb->reg); dmgb_tick(dmgb); break;
  case 0x24: dmgb->reg.H = dmgb_inc_u8(dmgb, dmgb->reg.H); break;
  case 0x25: dmgb->reg.H = dmgb_dec_u8(dmgb, dmgb->reg.H); break;
  case 0x26: dmgb->reg.H = dmgb_read_imm8(dmgb); break;
  case 0x27: dmgb_daa(dmgb); break;
  case 0x28: dmgb_jump_rotation(dmgb, (int8_t)dmgb_read_imm8(dmgb), (dmgb->reg.F & FLAG_Z) != 0); break;
  case 0x29: dmgb_add_r16(dmgb, rf_get_hl(&dmgb->reg)); break;
  case 0x2A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_inc_hl(&dmgb->reg)); break;
  case 0x2B: rf_dec_hl(&dmgb->reg); dmgb_tick(dmgb); break;
  case 0x2C: dmgb->reg.L = dmgb_inc_u8(dmgb, dmgb->reg.L); break;
  case 0x2D: dmgb->reg.L = dmgb_dec_u8(dmgb, dmgb->reg.L); break;
  case 0x2E: dmgb->reg.L = dmgb_read_imm8(dmgb); break;
  case 0x2F: dmgb->reg.A = ~dmgb->reg.A; dmgb->reg.F |= FLAG_N | FLAG_H; break;
  case 0x30: dmgb_jump_rotation(dmgb, (int8_t)dmgb_read_imm8(dmgb), (dmgb->reg.F & FLAG_C) == 0); break;
  case 0x31: dmgb->reg.SP = dmgb_read_imm16(dmgb); break;
  case 0x32: dmgb_write_u8(dmgb, rf_dec_hl(&dmgb->reg), dmgb->reg.A); break;
  case 0x33: dmgb->reg.SP++; dmgb_tick(dmgb); break;
  case 0x34: dmgb_write_hl_u8(dmgb, dmgb_inc_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x35: dmgb_write_hl_u8(dmgb, dmgb_dec_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x36: dmgb_write_hl_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0x37: dmgb->reg.F &= ~(FLAG_N | FLAG_H); dmgb->reg.F |= FLAG_C; break;
  case 0x38: dmgb_jump_rotation(dmgb, (int8_t)dmgb_read_imm8(dmgb), (dmgb->reg.F & FLAG_C) != 0); break;
  case 0x39: dmgb_add_r16(dmgb, dmgb->reg.SP); break;
  case 0x3A: dmgb->reg.A = dmgb_read_u8(dmgb, rf_dec_hl(&dmgb->reg)); break;
  case 0x3B: dmgb->reg.SP--; dmgb_tick(dmgb); break;
  case 0x3C: dmgb->reg.A = dmgb_inc_u8(dmgb, dmgb->reg.A); break;
  case 0x3D: dmgb->reg.A = dmgb_dec_u8(dmgb, dmgb->reg.A); break;
  case 0x3E: dmgb->reg.A = dmgb_read_imm8(dmgb); break;
  case 0x3F: dmgb->reg.F &= ~(FLAG_N | FLAG_H); dmgb->reg.F ^= FLAG_C; break;
  case 0x40: /* dmgb->reg.B = dmgb->reg.B; */ break;
  case 0x41: dmgb->reg.B = dmgb->reg.C; break;
  case 0x42: dmgb->reg.B = dmgb->reg.D; break;
  case 0x43: dmgb->reg.B = dmgb->reg.E; break;
  case 0x44: dmgb->reg.B = dmgb->reg.H; break;
  case 0x45: dmgb->reg.B = dmgb->reg.L; break;
  case 0x46: dmgb->reg.B = dmgb_read_hl_u8(dmgb); break;
  case 0x47: dmgb->reg.B = dmgb->reg.A; break;
  case 0x48: dmgb->reg.C = dmgb->reg.B; break;
  case 0x49: /* dmgb->reg.C = dmgb->reg.C; */ break;
  case 0x4A: dmgb->reg.C = dmgb->reg.D; break;
  case 0x4B: dmgb->reg.C = dmgb->reg.E; break;
  case 0x4C: dmgb->reg.C = dmgb->reg.H; break;
  case 0x4D: dmgb->reg.C = dmgb->reg.L; break;
  case 0x4E: dmgb->reg.C = dmgb_read_hl_u8(dmgb); break;
  case 0x4F: dmgb->reg.C = dmgb->reg.A; break;
  case 0x50: dmgb->reg.D = dmgb->reg.B; break;
  case 0x51: dmgb->reg.D = dmgb->reg.C; break;
  case 0x52: /* dmgb->reg.D = dmgb->reg.D; */ break;
  case 0x53: dmgb->reg.D = dmgb->reg.E; break;
  case 0x54: dmgb->reg.D = dmgb->reg.H; break;
  case 0x55: dmgb->reg.D = dmgb->reg.L; break;
  case 0x56: dmgb->reg.D = dmgb_read_hl_u8(dmgb); break;
  case 0x57: dmgb->reg.D = dmgb->reg.A; break;
  case 0x58: dmgb->reg.E = dmgb->reg.B; break;
  case 0x59: dmgb->reg.E = dmgb->reg.C; break;
  case 0x5A: dmgb->reg.E = dmgb->reg.D; break;
  case 0x5B: /* dmgb->reg.E = dmgb->reg.E; */ break;
  case 0x5C: dmgb->reg.E = dmgb->reg.H; break;
  case 0x5D: dmgb->reg.E = dmgb->reg.L; break;
  case 0x5E: dmgb->reg.E = dmgb_read_hl_u8(dmgb); break;
  case 0x5F: dmgb->reg.E = dmgb->reg.A; break;
  case 0x60: dmgb->reg.H = dmgb->reg.B; break;
  case 0x61: dmgb->reg.H = dmgb->reg.C; break;
  case 0x62: dmgb->reg.H = dmgb->reg.D; break;
  case 0x63: dmgb->reg.H = dmgb->reg.E; break;
  case 0x64: /* dmgb->reg.H = dmgb->reg.H; */ break;
  case 0x65: dmgb->reg.H = dmgb->reg.L; break;
  case 0x66: dmgb->reg.H = dmgb_read_hl_u8(dmgb); break;
  case 0x67: dmgb->reg.H = dmgb->reg.A; break;
  case 0x68: dmgb->reg.L = dmgb->reg.B; break;
  case 0x69: dmgb->reg.L = dmgb->reg.C; break;
  case 0x6A: dmgb->reg.L = dmgb->reg.D; break;
  case 0x6B: dmgb->reg.L = dmgb->reg.E; break;
  case 0x6C: dmgb->reg.L = dmgb->reg.H; break;
  case 0x6D: /* dmgb->reg.L = dmgb->reg.L; */ break;
  case 0x6E: dmgb->reg.L = dmgb_read_hl_u8(dmgb); break;
  case 0x6F: dmgb->reg.L = dmgb->reg.A; break;
  case 0x70: dmgb_write_hl_u8(dmgb, dmgb->reg.B); break;
  case 0x71: dmgb_write_hl_u8(dmgb, dmgb->reg.C); break;
  case 0x72: dmgb_write_hl_u8(dmgb, dmgb->reg.D); break;
  case 0x73: dmgb_write_hl_u8(dmgb, dmgb->reg.E); break;
  case 0x74: dmgb_write_hl_u8(dmgb, dmgb->reg.H); break;
  case 0x75: dmgb_write_hl_u8(dmgb, dmgb->reg.L); break;
  case 0x76: dmgb_halt(dmgb); break;
  case 0x77: dmgb_write_hl_u8(dmgb, dmgb->reg.A); break;
  case 0x78: dmgb->reg.A = dmgb->reg.B; break;
  case 0x79: dmgb->reg.A = dmgb->reg.C; break;
  case 0x7A: dmgb->reg.A = dmgb->reg.D; break;
  case 0x7B: dmgb->reg.A = dmgb->reg.E; break;
  case 0x7C: dmgb->reg.A = dmgb->reg.H; break;
  case 0x7D: dmgb->reg.A = dmgb->reg.L; break;
  case 0x7E: dmgb->reg.A = dmgb_read_hl_u8(dmgb); break;
  case 0x7F: /* dmgb->reg.A = dmgb->reg.A; */ break;
  case 0x80: dmgb_add_u8(dmgb, dmgb->reg.B); break;
  case 0x81: dmgb_add_u8(dmgb, dmgb->reg.C); break;
  case 0x82: dmgb_add_u8(dmgb, dmgb->reg.D); break;
  case 0x83: dmgb_add_u8(dmgb, dmgb->reg.E); break;
  case 0x84: dmgb_add_u8(dmgb, dmgb->reg.H); break;
  case 0x85: dmgb_add_u8(dmgb, dmgb->reg.L); break;
  case 0x86: dmgb_add_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0x87: dmgb_add_u8(dmgb, dmgb->reg.A); break;
  case 0x88: dmgb_adc_u8(dmgb, dmgb->reg.B); break;
  case 0x89: dmgb_adc_u8(dmgb, dmgb->reg.C); break;
  case 0x8A: dmgb_adc_u8(dmgb, dmgb->reg.D); break;
  case 0x8B: dmgb_adc_u8(dmgb, dmgb->reg.E); break;
  case 0x8C: dmgb_adc_u8(dmgb, dmgb->reg.H); break;
  case 0x8D: dmgb_adc_u8(dmgb, dmgb->reg.L); break;
  case 0x8E: dmgb_adc_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0x8F: dmgb_adc_u8(dmgb, dmgb->reg.A); break;
  case 0x90: dmgb_sub_u8(dmgb, dmgb->reg.B); break;
  case 0x91: dmgb_sub_u8(dmgb, dmgb->reg.C); break;
  case 0x92: dmgb_sub_u8(dmgb, dmgb->reg.D); break;
  case 0x93: dmgb_sub_u8(dmgb, dmgb->reg.E); break;
  case 0x94: dmgb_sub_u8(dmgb, dmgb->reg.H); break;
  case 0x95: dmgb_sub_u8(dmgb, dmgb->reg.L); break;
  case 0x96: dmgb_sub_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0x97: dmgb_sub_u8(dmgb, dmgb->reg.A); break;
  case 0x98: dmgb_sbc_u8(dmgb, dmgb->reg.B); break;
  case 0x99: dmgb_sbc_u8(dmgb, dmgb->reg.C); break;
  case 0x9A: dmgb_sbc_u8(dmgb, dmgb->reg.D); break;
  case 0x9B: dmgb_sbc_u8(dmgb, dmgb->reg.E); break;
  case 0x9C: dmgb_sbc_u8(dmgb, dmgb->reg.H); break;
  case 0x9D: dmgb_sbc_u8(dmgb, dmgb->reg.L); break;
  case 0x9E: dmgb_sbc_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0x9F: dmgb_sbc_u8(dmgb, dmgb->reg.A); break;
  case 0xA0: dmgb_and_u8(dmgb, dmgb->reg.B); break;
  case 0xA1: dmgb_and_u8(dmgb, dmgb->reg.C); break;
  case 0xA2: dmgb_and_u8(dmgb, dmgb->reg.D); break;
  case 0xA3: dmgb_and_u8(dmgb, dmgb->reg.E); break;
  case 0xA4: dmgb_and_u8(dmgb, dmgb->reg.H); break;
  case 0xA5: dmgb_and_u8(dmgb, dmgb->reg.L); break;
  case 0xA6: dmgb_and_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0xA7: dmgb_and_u8(dmgb, dmgb->reg.A); break;
  case 0xA8: dmgb_xor_u8(dmgb, dmgb->reg.B); break;
  case 0xA9: dmgb_xor_u8(dmgb, dmgb->reg.C); break;
  case 0xAA: dmgb_xor_u8(dmgb, dmgb->reg.D); break;
  case 0xAB: dmgb_xor_u8(dmgb, dmgb->reg.E); break;
  case 0xAC: dmgb_xor_u8(dmgb, dmgb->reg.H); break;
  case 0xAD: dmgb_xor_u8(dmgb, dmgb->reg.L); break;
  case 0xAE: dmgb_xor_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0xAF: dmgb_xor_u8(dmgb, dmgb->reg.A); break;
  case 0xB0: dmgb_or_u8(dmgb, dmgb->reg.B); break;
  case 0xB1: dmgb_or_u8(dmgb, dmgb->reg.C); break;
  case 0xB2: dmgb_or_u8(dmgb, dmgb->reg.D); break;
  case 0xB3: dmgb_or_u8(dmgb, dmgb->reg.E); break;
  case 0xB4: dmgb_or_u8(dmgb, dmgb->reg.H); break;
  case 0xB5: dmgb_or_u8(dmgb, dmgb->reg.L); break;
  case 0xB6: dmgb_or_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0xB7: dmgb_or_u8(dmgb, dmgb->reg.A); break;
  case 0xB8: dmgb_cp_u8(dmgb, dmgb->reg.B); break;
  case 0xB9: dmgb_cp_u8(dmgb, dmgb->reg.C); break;
  case 0xBA: dmgb_cp_u8(dmgb, dmgb->reg.D); break;
  case 0xBB: dmgb_cp_u8(dmgb, dmgb->reg.E); break;
  case 0xBC: dmgb_cp_u8(dmgb, dmgb->reg.H); break;
  case 0xBD: dmgb_cp_u8(dmgb, dmgb->reg.L); break;
  case 0xBE: dmgb_cp_u8(dmgb, dmgb_read_hl_u8(dmgb)); break;
  case 0xBF: dmgb_cp_u8(dmgb, dmgb->reg.A); break;
  case 0xC0: dmgb_tick(dmgb); dmgb_return(dmgb, (dmgb->reg.F & FLAG_Z) == 0); break;
  case 0xC1: rf_set_bc(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
  case 0xC2: dmgb_jump(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_Z) == 0); break;
  case 0xC3: dmgb_jump(dmgb, dmgb_read_imm16(dmgb), true); break;
  case 0xC4: dmgb_call(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_Z) == 0); break;
  case 0xC5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_bc(&dmgb->reg)); break;
  case 0xC6: dmgb_add_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xC7: dmgb_call(dmgb, 0x00, true); break;
  case 0xC8: dmgb_tick(dmgb); dmgb_return(dmgb, (dmgb->reg.F & FLAG_Z) != 0); break;
  case 0xC9: dmgb_return(dmgb, true); break;
  case 0xCA: dmgb_jump(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_Z) != 0); break;
  case 0xCB: dmgb_execute_cb(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xCC: dmgb_call(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_Z) != 0); break;
  case 0xCD: dmgb_call(dmgb, dmgb_read_imm16(dmgb), true); break;
  case 0xCE: dmgb_adc_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xCF: dmgb_call(dmgb, 0x08, true); break;
  case 0xD0: dmgb_tick(dmgb); dmgb_return(dmgb, (dmgb->reg.F & FLAG_C) == 0); break;
  case 0xD1: rf_set_de(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
  case 0xD2: dmgb_jump(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_C) == 0); break;
  case 0xD3: dmgb_illegal(dmgb, opcode); break;
  case 0xD4: dmgb_call(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_C) == 0); break;
  case 0xD5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_de(&dmgb->reg)); break;
  case 0xD6: dmgb_sub_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xD7: dmgb_call(dmgb, 0x10, true); break;
  case 0xD8: dmgb_tick(dmgb); dmgb_return(dmgb, (dmgb->reg.F & FLAG_C) != 0); break;
  case 0xD9: dmgb_return(dmgb, dmgb->ime_pending = true); break;
  case 0xDA: dmgb_jump(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_C) != 0); break;
  case 0xDB: dmgb_illegal(dmgb, opcode); break;
  case 0xDC: dmgb_call(dmgb, dmgb_read_imm16(dmgb), (dmgb->reg.F & FLAG_C) != 0); break;
  case 0xDD: dmgb_illegal(dmgb, opcode); break;
  case 0xDE: dmgb_sbc_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xDF: dmgb_call(dmgb, 0x18, true); break;
  case 0xE0: dmgb_write_u8(dmgb, 0xFF00 + dmgb_read_imm8(dmgb), dmgb->reg.A); break;
  case 0xE1: rf_set_hl(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
  case 0xE2: dmgb_write_u8(dmgb, 0xFF00 + dmgb->reg.C, dmgb->reg.A); break;
  case 0xE3: dmgb_illegal(dmgb, opcode); break;
  case 0xE4: dmgb_illegal(dmgb, opcode); break;
  case 0xE5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_hl(&dmgb->reg)); break;
  case 0xE6: dmgb_and_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xE7: dmgb_call(dmgb, 0x20, true); break;
  case 0xE8: dmgb->reg.SP = dmgb_add_sp_i8(dmgb, (int8_t)dmgb_read_imm8(dmgb)); dmgb_tick(dmgb); break;
  case 0xE9: dmgb->reg.PC = rf_get_hl(&dmgb->reg); break;
  case 0xEA: dmgb_write_u8(dmgb, dmgb_read_imm16(dmgb), dmgb->reg.A); break;
  case 0xEB: dmgb_illegal(dmgb, opcode); break;
  case 0xEC: dmgb_illegal(dmgb, opcode); break;
  case 0xED: dmgb_illegal(dmgb, opcode); break;
  case 0xEE: dmgb_xor_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xEF: dmgb_call(dmgb, 0x28, true); break;
  case 0xF0: dmgb->reg.A = dmgb_read_u8(dmgb, 0xFF00 + dmgb_read_imm8(dmgb)); break;
  case 0xF1: rf_set_af(&dmgb->reg, dmgb_pop_u16(dmgb)); break;
  case 0xF2: dmgb->reg.A = dmgb_read_u8(dmgb, 0xFF00 + dmgb->reg.C); break;
  case 0xF3: dmgb->reg.IME = dmgb->ime_pending = false; break;
  case 0xF4: dmgb_illegal(dmgb, opcode); break;
  case 0xF5: dmgb_tick(dmgb); dmgb_push_u16(dmgb, rf_get_af(&dmgb->reg)); break;
  case 0xF6: dmgb_or_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xF7: dmgb_call(dmgb, 0x30, true); break;
  case 0xF8: rf_set_hl(&dmgb->reg, dmgb_add_sp_i8(dmgb, (int8_t)dmgb_read_imm8(dmgb))); break;
  case 0xF9: dmgb->reg.SP = rf_get_hl(&dmgb->reg); dmgb_tick(dmgb); break;
  case 0xFA: dmgb->reg.A = dmgb_read_u8(dmgb, dmgb_read_imm16(dmgb)); break;
  case 0xFB: dmgb->ime_pending = true; break;
  case 0xFC: dmgb_illegal(dmgb, opcode); break;
  case 0xFD: dmgb_illegal(dmgb, opcode); break;
  case 0xFE: dmgb_cp_u8(dmgb, dmgb_read_imm8(dmgb)); break;
  case 0xFF: dmgb_call(dmgb, 0x38, true); break;
    // clang-format on
  }

#if !defined(NDEBUG) && !SUPPRESS_LOGS
  dmgb_log_state(dmgb, opcode, mnemonics[opcode]);
#endif
}

void dmgb_execute_cb(struct dmgb *dmgb, uint8_t opcode) {
  switch (opcode) {
    // clang-format off
  case 0x00: dmgb->reg.B = dmgb_rlc_u8(dmgb, dmgb->reg.B); break;
  case 0x01: dmgb->reg.C = dmgb_rlc_u8(dmgb, dmgb->reg.C); break;
  case 0x02: dmgb->reg.D = dmgb_rlc_u8(dmgb, dmgb->reg.D); break;
  case 0x03: dmgb->reg.E = dmgb_rlc_u8(dmgb, dmgb->reg.E); break;
  case 0x04: dmgb->reg.H = dmgb_rlc_u8(dmgb, dmgb->reg.H); break;
  case 0x05: dmgb->reg.L = dmgb_rlc_u8(dmgb, dmgb->reg.L); break;
  case 0x06: dmgb_write_hl_u8(dmgb, dmgb_rlc_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x07: dmgb->reg.A = dmgb_rlc_u8(dmgb, dmgb->reg.A); break;
  case 0x08: dmgb->reg.B = dmgb_rrc_u8(dmgb, dmgb->reg.B); break;
  case 0x09: dmgb->reg.C = dmgb_rrc_u8(dmgb, dmgb->reg.C); break;
  case 0x0A: dmgb->reg.D = dmgb_rrc_u8(dmgb, dmgb->reg.D); break;
  case 0x0B: dmgb->reg.E = dmgb_rrc_u8(dmgb, dmgb->reg.E); break;
  case 0x0C: dmgb->reg.H = dmgb_rrc_u8(dmgb, dmgb->reg.H); break;
  case 0x0D: dmgb->reg.L = dmgb_rrc_u8(dmgb, dmgb->reg.L); break;
  case 0x0E: dmgb_write_hl_u8(dmgb, dmgb_rrc_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x0F: dmgb->reg.A = dmgb_rrc_u8(dmgb, dmgb->reg.A); break;
  case 0x10: dmgb->reg.B = dmgb_rl_u8(dmgb, dmgb->reg.B); break;
  case 0x11: dmgb->reg.C = dmgb_rl_u8(dmgb, dmgb->reg.C); break;
  case 0x12: dmgb->reg.D = dmgb_rl_u8(dmgb, dmgb->reg.D); break;
  case 0x13: dmgb->reg.E = dmgb_rl_u8(dmgb, dmgb->reg.E); break;
  case 0x14: dmgb->reg.H = dmgb_rl_u8(dmgb, dmgb->reg.H); break;
  case 0x15: dmgb->reg.L = dmgb_rl_u8(dmgb, dmgb->reg.L); break;
  case 0x16: dmgb_write_hl_u8(dmgb, dmgb_rl_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x17: dmgb->reg.A = dmgb_rl_u8(dmgb, dmgb->reg.A); break;
  case 0x18: dmgb->reg.B = dmgb_rr_u8(dmgb, dmgb->reg.B); break;
  case 0x19: dmgb->reg.C = dmgb_rr_u8(dmgb, dmgb->reg.C); break;
  case 0x1A: dmgb->reg.D = dmgb_rr_u8(dmgb, dmgb->reg.D); break;
  case 0x1B: dmgb->reg.E = dmgb_rr_u8(dmgb, dmgb->reg.E); break;
  case 0x1C: dmgb->reg.H = dmgb_rr_u8(dmgb, dmgb->reg.H); break;
  case 0x1D: dmgb->reg.L = dmgb_rr_u8(dmgb, dmgb->reg.L); break;
  case 0x1E: dmgb_write_hl_u8(dmgb, dmgb_rr_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x1F: dmgb->reg.A = dmgb_rr_u8(dmgb, dmgb->reg.A); break;
  case 0x20: dmgb->reg.B = dmgb_sla_u8(dmgb, dmgb->reg.B); break;
  case 0x21: dmgb->reg.C = dmgb_sla_u8(dmgb, dmgb->reg.C); break;
  case 0x22: dmgb->reg.D = dmgb_sla_u8(dmgb, dmgb->reg.D); break;
  case 0x23: dmgb->reg.E = dmgb_sla_u8(dmgb, dmgb->reg.E); break;
  case 0x24: dmgb->reg.H = dmgb_sla_u8(dmgb, dmgb->reg.H); break;
  case 0x25: dmgb->reg.L = dmgb_sla_u8(dmgb, dmgb->reg.L); break;
  case 0x26: dmgb_write_hl_u8(dmgb, dmgb_sla_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x27: dmgb->reg.A = dmgb_sla_u8(dmgb, dmgb->reg.A); break;
  case 0x28: dmgb->reg.B = dmgb_sra_u8(dmgb, dmgb->reg.B); break;
  case 0x29: dmgb->reg.C = dmgb_sra_u8(dmgb, dmgb->reg.C); break;
  case 0x2A: dmgb->reg.D = dmgb_sra_u8(dmgb, dmgb->reg.D); break;
  case 0x2B: dmgb->reg.E = dmgb_sra_u8(dmgb, dmgb->reg.E); break;
  case 0x2C: dmgb->reg.H = dmgb_sra_u8(dmgb, dmgb->reg.H); break;
  case 0x2D: dmgb->reg.L = dmgb_sra_u8(dmgb, dmgb->reg.L); break;
  case 0x2E: dmgb_write_hl_u8(dmgb, dmgb_sra_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x2F: dmgb->reg.A = dmgb_sra_u8(dmgb, dmgb->reg.A); break;
  case 0x30: dmgb->reg.B = dmgb_swap_u8(dmgb, dmgb->reg.B); break;
  case 0x31: dmgb->reg.C = dmgb_swap_u8(dmgb, dmgb->reg.C); break;
  case 0x32: dmgb->reg.D = dmgb_swap_u8(dmgb, dmgb->reg.D); break;
  case 0x33: dmgb->reg.E = dmgb_swap_u8(dmgb, dmgb->reg.E); break;
  case 0x34: dmgb->reg.H = dmgb_swap_u8(dmgb, dmgb->reg.H); break;
  case 0x35: dmgb->reg.L = dmgb_swap_u8(dmgb, dmgb->reg.L); break;
  case 0x36: dmgb_write_hl_u8(dmgb, dmgb_swap_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x37: dmgb->reg.A = dmgb_swap_u8(dmgb, dmgb->reg.A); break;
  case 0x38: dmgb->reg.B = dmgb_srl_u8(dmgb, dmgb->reg.B); break;
  case 0x39: dmgb->reg.C = dmgb_srl_u8(dmgb, dmgb->reg.C); break;
  case 0x3A: dmgb->reg.D = dmgb_srl_u8(dmgb, dmgb->reg.D); break;
  case 0x3B: dmgb->reg.E = dmgb_srl_u8(dmgb, dmgb->reg.E); break;
  case 0x3C: dmgb->reg.H = dmgb_srl_u8(dmgb, dmgb->reg.H); break;
  case 0x3D: dmgb->reg.L = dmgb_srl_u8(dmgb, dmgb->reg.L); break;
  case 0x3E: dmgb_write_hl_u8(dmgb, dmgb_srl_u8(dmgb, dmgb_read_hl_u8(dmgb))); break;
  case 0x3F: dmgb->reg.A = dmgb_srl_u8(dmgb, dmgb->reg.A); break;
  case 0x40: dmgb_bit_b3_u8(dmgb, 0, dmgb->reg.B); break;
  case 0x41: dmgb_bit_b3_u8(dmgb, 0, dmgb->reg.C); break;
  case 0x42: dmgb_bit_b3_u8(dmgb, 0, dmgb->reg.D); break;
  case 0x43: dmgb_bit_b3_u8(dmgb, 0, dmgb->reg.E); break;
  case 0x44: dmgb_bit_b3_u8(dmgb, 0, dmgb->reg.H); break;
  case 0x45: dmgb_bit_b3_u8(dmgb, 0, dmgb->reg.L); break;
  case 0x46: dmgb_bit_b3_u8(dmgb, 0, dmgb_read_hl_u8(dmgb)); break;
  case 0x47: dmgb_bit_b3_u8(dmgb, 0, dmgb->reg.A); break;
  case 0x48: dmgb_bit_b3_u8(dmgb, 1, dmgb->reg.B); break;
  case 0x49: dmgb_bit_b3_u8(dmgb, 1, dmgb->reg.C); break;
  case 0x4A: dmgb_bit_b3_u8(dmgb, 1, dmgb->reg.D); break;
  case 0x4B: dmgb_bit_b3_u8(dmgb, 1, dmgb->reg.E); break;
  case 0x4C: dmgb_bit_b3_u8(dmgb, 1, dmgb->reg.H); break;
  case 0x4D: dmgb_bit_b3_u8(dmgb, 1, dmgb->reg.L); break;
  case 0x4E: dmgb_bit_b3_u8(dmgb, 1, dmgb_read_hl_u8(dmgb)); break;
  case 0x4F: dmgb_bit_b3_u8(dmgb, 1, dmgb->reg.A); break;
  case 0x50: dmgb_bit_b3_u8(dmgb, 2, dmgb->reg.B); break;
  case 0x51: dmgb_bit_b3_u8(dmgb, 2, dmgb->reg.C); break;
  case 0x52: dmgb_bit_b3_u8(dmgb, 2, dmgb->reg.D); break;
  case 0x53: dmgb_bit_b3_u8(dmgb, 2, dmgb->reg.E); break;
  case 0x54: dmgb_bit_b3_u8(dmgb, 2, dmgb->reg.H); break;
  case 0x55: dmgb_bit_b3_u8(dmgb, 2, dmgb->reg.L); break;
  case 0x56: dmgb_bit_b3_u8(dmgb, 2, dmgb_read_hl_u8(dmgb)); break;
  case 0x57: dmgb_bit_b3_u8(dmgb, 2, dmgb->reg.A); break;
  case 0x58: dmgb_bit_b3_u8(dmgb, 3, dmgb->reg.B); break;
  case 0x59: dmgb_bit_b3_u8(dmgb, 3, dmgb->reg.C); break;
  case 0x5A: dmgb_bit_b3_u8(dmgb, 3, dmgb->reg.D); break;
  case 0x5B: dmgb_bit_b3_u8(dmgb, 3, dmgb->reg.E); break;
  case 0x5C: dmgb_bit_b3_u8(dmgb, 3, dmgb->reg.H); break;
  case 0x5D: dmgb_bit_b3_u8(dmgb, 3, dmgb->reg.L); break;
  case 0x5E: dmgb_bit_b3_u8(dmgb, 3, dmgb_read_hl_u8(dmgb)); break;
  case 0x5F: dmgb_bit_b3_u8(dmgb, 3, dmgb->reg.A); break;
  case 0x60: dmgb_bit_b3_u8(dmgb, 4, dmgb->reg.B); break;
  case 0x61: dmgb_bit_b3_u8(dmgb, 4, dmgb->reg.C); break;
  case 0x62: dmgb_bit_b3_u8(dmgb, 4, dmgb->reg.D); break;
  case 0x63: dmgb_bit_b3_u8(dmgb, 4, dmgb->reg.E); break;
  case 0x64: dmgb_bit_b3_u8(dmgb, 4, dmgb->reg.H); break;
  case 0x65: dmgb_bit_b3_u8(dmgb, 4, dmgb->reg.L); break;
  case 0x66: dmgb_bit_b3_u8(dmgb, 4, dmgb_read_hl_u8(dmgb)); break;
  case 0x67: dmgb_bit_b3_u8(dmgb, 4, dmgb->reg.A); break;
  case 0x68: dmgb_bit_b3_u8(dmgb, 5, dmgb->reg.B); break;
  case 0x69: dmgb_bit_b3_u8(dmgb, 5, dmgb->reg.C); break;
  case 0x6A: dmgb_bit_b3_u8(dmgb, 5, dmgb->reg.D); break;
  case 0x6B: dmgb_bit_b3_u8(dmgb, 5, dmgb->reg.E); break;
  case 0x6C: dmgb_bit_b3_u8(dmgb, 5, dmgb->reg.H); break;
  case 0x6D: dmgb_bit_b3_u8(dmgb, 5, dmgb->reg.L); break;
  case 0x6E: dmgb_bit_b3_u8(dmgb, 5, dmgb_read_hl_u8(dmgb)); break;
  case 0x6F: dmgb_bit_b3_u8(dmgb, 5, dmgb->reg.A); break;
  case 0x70: dmgb_bit_b3_u8(dmgb, 6, dmgb->reg.B); break;
  case 0x71: dmgb_bit_b3_u8(dmgb, 6, dmgb->reg.C); break;
  case 0x72: dmgb_bit_b3_u8(dmgb, 6, dmgb->reg.D); break;
  case 0x73: dmgb_bit_b3_u8(dmgb, 6, dmgb->reg.E); break;
  case 0x74: dmgb_bit_b3_u8(dmgb, 6, dmgb->reg.H); break;
  case 0x75: dmgb_bit_b3_u8(dmgb, 6, dmgb->reg.L); break;
  case 0x76: dmgb_bit_b3_u8(dmgb, 6, dmgb_read_hl_u8(dmgb)); break;
  case 0x77: dmgb_bit_b3_u8(dmgb, 6, dmgb->reg.A); break;
  case 0x78: dmgb_bit_b3_u8(dmgb, 7, dmgb->reg.B); break;
  case 0x79: dmgb_bit_b3_u8(dmgb, 7, dmgb->reg.C); break;
  case 0x7A: dmgb_bit_b3_u8(dmgb, 7, dmgb->reg.D); break;
  case 0x7B: dmgb_bit_b3_u8(dmgb, 7, dmgb->reg.E); break;
  case 0x7C: dmgb_bit_b3_u8(dmgb, 7, dmgb->reg.H); break;
  case 0x7D: dmgb_bit_b3_u8(dmgb, 7, dmgb->reg.L); break;
  case 0x7E: dmgb_bit_b3_u8(dmgb, 7, dmgb_read_hl_u8(dmgb)); break;
  case 0x7F: dmgb_bit_b3_u8(dmgb, 7, dmgb->reg.A); break;
  case 0x80: dmgb->reg.B &= ~(1 << 0); break;
  case 0x81: dmgb->reg.C &= ~(1 << 0); break;
  case 0x82: dmgb->reg.D &= ~(1 << 0); break;
  case 0x83: dmgb->reg.E &= ~(1 << 0); break;
  case 0x84: dmgb->reg.H &= ~(1 << 0); break;
  case 0x85: dmgb->reg.L &= ~(1 << 0); break;
  case 0x86: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 0)); break;
  case 0x87: dmgb->reg.A &= ~(1 << 0); break;
  case 0x88: dmgb->reg.B &= ~(1 << 1); break;
  case 0x89: dmgb->reg.C &= ~(1 << 1); break;
  case 0x8A: dmgb->reg.D &= ~(1 << 1); break;
  case 0x8B: dmgb->reg.E &= ~(1 << 1); break;
  case 0x8C: dmgb->reg.H &= ~(1 << 1); break;
  case 0x8D: dmgb->reg.L &= ~(1 << 1); break;
  case 0x8E: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 1)); break;
  case 0x8F: dmgb->reg.A &= ~(1 << 1); break;
  case 0x90: dmgb->reg.B &= ~(1 << 2); break;
  case 0x91: dmgb->reg.C &= ~(1 << 2); break;
  case 0x92: dmgb->reg.D &= ~(1 << 2); break;
  case 0x93: dmgb->reg.E &= ~(1 << 2); break;
  case 0x94: dmgb->reg.H &= ~(1 << 2); break;
  case 0x95: dmgb->reg.L &= ~(1 << 2); break;
  case 0x96: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 2)); break;
  case 0x97: dmgb->reg.A &= ~(1 << 2); break;
  case 0x98: dmgb->reg.B &= ~(1 << 3); break;
  case 0x99: dmgb->reg.C &= ~(1 << 3); break;
  case 0x9A: dmgb->reg.D &= ~(1 << 3); break;
  case 0x9B: dmgb->reg.E &= ~(1 << 3); break;
  case 0x9C: dmgb->reg.H &= ~(1 << 3); break;
  case 0x9D: dmgb->reg.L &= ~(1 << 3); break;
  case 0x9E: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 3)); break;
  case 0x9F: dmgb->reg.A &= ~(1 << 3); break;
  case 0xA0: dmgb->reg.B &= ~(1 << 4); break;
  case 0xA1: dmgb->reg.C &= ~(1 << 4); break;
  case 0xA2: dmgb->reg.D &= ~(1 << 4); break;
  case 0xA3: dmgb->reg.E &= ~(1 << 4); break;
  case 0xA4: dmgb->reg.H &= ~(1 << 4); break;
  case 0xA5: dmgb->reg.L &= ~(1 << 4); break;
  case 0xA6: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 4)); break;
  case 0xA7: dmgb->reg.A &= ~(1 << 4); break;
  case 0xA8: dmgb->reg.B &= ~(1 << 5); break;
  case 0xA9: dmgb->reg.C &= ~(1 << 5); break;
  case 0xAA: dmgb->reg.D &= ~(1 << 5); break;
  case 0xAB: dmgb->reg.E &= ~(1 << 5); break;
  case 0xAC: dmgb->reg.H &= ~(1 << 5); break;
  case 0xAD: dmgb->reg.L &= ~(1 << 5); break;
  case 0xAE: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 5)); break;
  case 0xAF: dmgb->reg.A &= ~(1 << 5); break;
  case 0xB0: dmgb->reg.B &= ~(1 << 6); break;
  case 0xB1: dmgb->reg.C &= ~(1 << 6); break;
  case 0xB2: dmgb->reg.D &= ~(1 << 6); break;
  case 0xB3: dmgb->reg.E &= ~(1 << 6); break;
  case 0xB4: dmgb->reg.H &= ~(1 << 6); break;
  case 0xB5: dmgb->reg.L &= ~(1 << 6); break;
  case 0xB6: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 6)); break;
  case 0xB7: dmgb->reg.A &= ~(1 << 6); break;
  case 0xB8: dmgb->reg.B &= ~(1 << 7); break;
  case 0xB9: dmgb->reg.C &= ~(1 << 7); break;
  case 0xBA: dmgb->reg.D &= ~(1 << 7); break;
  case 0xBB: dmgb->reg.E &= ~(1 << 7); break;
  case 0xBC: dmgb->reg.H &= ~(1 << 7); break;
  case 0xBD: dmgb->reg.L &= ~(1 << 7); break;
  case 0xBE: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) & ~(1 << 7)); break;
  case 0xBF: dmgb->reg.A &= ~(1 << 7); break;
  case 0xC0: dmgb->reg.B |= 1 << 0; break;
  case 0xC1: dmgb->reg.C |= 1 << 0; break;
  case 0xC2: dmgb->reg.D |= 1 << 0; break;
  case 0xC3: dmgb->reg.E |= 1 << 0; break;
  case 0xC4: dmgb->reg.H |= 1 << 0; break;
  case 0xC5: dmgb->reg.L |= 1 << 0; break;
  case 0xC6: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 0); break;
  case 0xC7: dmgb->reg.A |= 1 << 0; break;
  case 0xC8: dmgb->reg.B |= 1 << 1; break;
  case 0xC9: dmgb->reg.C |= 1 << 1; break;
  case 0xCA: dmgb->reg.D |= 1 << 1; break;
  case 0xCB: dmgb->reg.E |= 1 << 1; break;
  case 0xCC: dmgb->reg.H |= 1 << 1; break;
  case 0xCD: dmgb->reg.L |= 1 << 1; break;
  case 0xCE: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 1); break;
  case 0xCF: dmgb->reg.A |= 1 << 1; break;
  case 0xD0: dmgb->reg.B |= 1 << 2; break;
  case 0xD1: dmgb->reg.C |= 1 << 2; break;
  case 0xD2: dmgb->reg.D |= 1 << 2; break;
  case 0xD3: dmgb->reg.E |= 1 << 2; break;
  case 0xD4: dmgb->reg.H |= 1 << 2; break;
  case 0xD5: dmgb->reg.L |= 1 << 2; break;
  case 0xD6: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 2); break;
  case 0xD7: dmgb->reg.A |= 1 << 2; break;
  case 0xD8: dmgb->reg.B |= 1 << 3; break;
  case 0xD9: dmgb->reg.C |= 1 << 3; break;
  case 0xDA: dmgb->reg.D |= 1 << 3; break;
  case 0xDB: dmgb->reg.E |= 1 << 3; break;
  case 0xDC: dmgb->reg.H |= 1 << 3; break;
  case 0xDD: dmgb->reg.L |= 1 << 3; break;
  case 0xDE: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 3); break;
  case 0xDF: dmgb->reg.A |= 1 << 3; break;
  case 0xE0: dmgb->reg.B |= 1 << 4; break;
  case 0xE1: dmgb->reg.C |= 1 << 4; break;
  case 0xE2: dmgb->reg.D |= 1 << 4; break;
  case 0xE3: dmgb->reg.E |= 1 << 4; break;
  case 0xE4: dmgb->reg.H |= 1 << 4; break;
  case 0xE5: dmgb->reg.L |= 1 << 4; break;
  case 0xE6: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 4); break;
  case 0xE7: dmgb->reg.A |= 1 << 4; break;
  case 0xE8: dmgb->reg.B |= 1 << 5; break;
  case 0xE9: dmgb->reg.C |= 1 << 5; break;
  case 0xEA: dmgb->reg.D |= 1 << 5; break;
  case 0xEB: dmgb->reg.E |= 1 << 5; break;
  case 0xEC: dmgb->reg.H |= 1 << 5; break;
  case 0xED: dmgb->reg.L |= 1 << 5; break;
  case 0xEE: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 5); break;
  case 0xEF: dmgb->reg.A |= 1 << 5; break;
  case 0xF0: dmgb->reg.B |= 1 << 6; break;
  case 0xF1: dmgb->reg.C |= 1 << 6; break;
  case 0xF2: dmgb->reg.D |= 1 << 6; break;
  case 0xF3: dmgb->reg.E |= 1 << 6; break;
  case 0xF4: dmgb->reg.H |= 1 << 6; break;
  case 0xF5: dmgb->reg.L |= 1 << 6; break;
  case 0xF6: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 6); break;
  case 0xF7: dmgb->reg.A |= 1 << 6; break;
  case 0xF8: dmgb->reg.B |= 1 << 7; break;
  case 0xF9: dmgb->reg.C |= 1 << 7; break;
  case 0xFA: dmgb->reg.D |= 1 << 7; break;
  case 0xFB: dmgb->reg.E |= 1 << 7; break;
  case 0xFC: dmgb->reg.H |= 1 << 7; break;
  case 0xFD: dmgb->reg.L |= 1 << 7; break;
  case 0xFE: dmgb_write_hl_u8(dmgb, dmgb_read_hl_u8(dmgb) | 1 << 7); break;
  case 0xFF: dmgb->reg.A |= 1 << 7; break;
    // clang-format on
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
  printf(
      "\n"
      "%02X: %s\n"
      "AF:%04X BC:%04X DE:%04X HL:%04X SP:%04X PC:%04X [BC]:%02X [DE]:%02X [HL]:%02X [SP]:%02X [PC]:%02X,%02X,%02X,%02X\n"
      "\n",
      opcode, mnemonic, AF, BC, DE, HL, SP, PC, dmgb_bus_read(dmgb, BC),
      dmgb_bus_read(dmgb, DE), dmgb_bus_read(dmgb, HL), dmgb_bus_read(dmgb, SP),
      dmgb_bus_read(dmgb, PC), dmgb_bus_read(dmgb, PC + 1),
      dmgb_bus_read(dmgb, PC + 2), dmgb_bus_read(dmgb, PC + 3));
}
