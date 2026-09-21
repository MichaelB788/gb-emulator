#include "cpu.h"
#include "bus.h"
#include "cpu_debugger.h"
#include "cpu_instrs.h"
#include "impl_cpu_instrs.h"
#include "instruction.h"
#include "interrupts.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

void cpu_init(struct cpu *cpu, struct bus *bus) {
  assert(bus != nullptr);
  cpu->bus = bus;

  cpu->state = CPU_RUNNING;

  cpu->PC = 0x100;
  cpu->AF = cpu->BC = cpu->DE = cpu->HL = cpu->SP = 0;

  cpu->IME = cpu->ime_pending = false;
}

void cpu_step(struct cpu *cpu) {
  if (cpu->ime_pending) {
    cpu->ime_pending = false;
    cpu->IME = true;
  }

  switch (cpu->state) {
  case CPU_RUNNING:
    cpu_execute(cpu, cpu_read_imm8(cpu));
    break;
  case CPU_HALTED:
    bus_tick(cpu->bus);
    break;
  case CPU_HALT_BUG:
    cpu->state = CPU_RUNNING;
    cpu_execute(cpu, cpu_read_u8(cpu, cpu->PC));
    break;
  }

  struct interrupts *in = &cpu->bus->interrupts;
  if (in->IE & in->IF) {
    cpu->state = CPU_RUNNING;
    if (cpu->IME)
      interrupts_service_pending(in, cpu);
  }
}

static void cpu_log_step_brief(const struct cpu *cpu) {
  printf(
      "AF:%04X BC:%04X DE:%04X HL:%04X SP:%04X PC:%04X [PC]:%02X,%02X,%02X,%02X\n",
      cpu->AF, cpu->BC, cpu->DE, cpu->HL, cpu->SP, cpu->PC,
      bus_read_byte(cpu->bus, cpu->PC), bus_read_byte(cpu->bus, cpu->PC + 1),
      bus_read_byte(cpu->bus, cpu->PC + 2),
      bus_read_byte(cpu->bus, cpu->PC + 3));
}

static void cpu_log_step_verbose(const struct cpu *cpu,
                                 const struct instruction *instr) {
  printf(
      "%02X: %s\n"
      "AF:%04X BC:%04X DE:%04X HL:%04X SP:%04X PC:%04X [BC]:%02X [DE]:%02X [HL]:%02X [SP]:%02X [PC]:%02X,%02X,%02X,%02X\n"
      "\n",
      instr->opcode, instr->mnemonic, cpu->AF, cpu->BC, cpu->DE, cpu->HL,
      cpu->SP, cpu->PC, bus_read_byte(cpu->bus, cpu->BC),
      bus_read_byte(cpu->bus, cpu->DE), bus_read_byte(cpu->bus, cpu->HL),
      bus_read_byte(cpu->bus, cpu->SP), bus_read_byte(cpu->bus, cpu->PC),
      bus_read_byte(cpu->bus, cpu->PC + 1),
      bus_read_byte(cpu->bus, cpu->PC + 2),
      bus_read_byte(cpu->bus, cpu->PC + 3));
}

void cpu_execute(struct cpu *cpu, uint8_t opcode) {
  switch (opcode) {
    // clang-format off
  case 0x00: /* NOP */ break;
  case 0x01: cpu->BC = cpu_read_imm16(cpu); break;
  case 0x02: cpu_write_u8(cpu, cpu->BC, cpu->A); break;
  case 0x03: cpu->BC++; bus_tick(cpu->bus); break;
  case 0x04: cpu->B = impl_inc_u8(cpu, cpu->B); break;
  case 0x05: cpu->B = impl_dec_u8(cpu, cpu->B); break;
  case 0x06: cpu->B = cpu_read_imm8(cpu); break;
  case 0x07: cpu->A = impl_rlc(cpu, cpu->A); cpu->F &= ~FLAG_Z; break;
  case 0x08: cpu_write_u16(cpu, cpu_read_imm16(cpu), cpu->SP); break;
  case 0x09: impl_add_r16(cpu, cpu->BC); break;
  case 0x0A: cpu->A = cpu_read_u8(cpu, cpu->BC); break;
  case 0x0B: cpu->BC--; bus_tick(cpu->bus); break;
  case 0x0C: cpu->C = impl_inc_u8(cpu, cpu->C); break;
  case 0x0D: cpu->C = impl_dec_u8(cpu, cpu->C); break;
  case 0x0E: cpu->C = cpu_read_imm8(cpu); break;
  case 0x0F: cpu->A = impl_rrc(cpu, cpu->A); cpu->F &= ~FLAG_Z; break;
  case 0x10: assert(false && opcode); break; // WARN: unimplemented
  case 0x11: cpu->DE = cpu_read_imm16(cpu); break;
  case 0x12: cpu_write_u8(cpu, cpu->DE, cpu->A); break;
  case 0x13: cpu->DE++; bus_tick(cpu->bus); break;
  case 0x14: cpu->D = impl_inc_u8(cpu, cpu->D); break;
  case 0x15: cpu->D = impl_dec_u8(cpu, cpu->D); break;
  case 0x16: cpu->D = cpu_read_imm8(cpu); break;
  case 0x17: cpu->A = impl_rl(cpu, cpu->A); cpu->F &= ~FLAG_Z; break;
  case 0x18: cpu_jump_rotation(cpu, cpu_read_imm8(cpu), true); break;
  case 0x19: impl_add_r16(cpu, cpu->DE); break;
  case 0x1A: cpu->A = cpu_read_u8(cpu, cpu->DE); break;
  case 0x1B: cpu->DE--; bus_tick(cpu->bus); break;
  case 0x1C: cpu->E = impl_inc_u8(cpu, cpu->E); break;
  case 0x1D: cpu->E = impl_dec_u8(cpu, cpu->E); break;
  case 0x1E: cpu->E = cpu_read_imm8(cpu); break;
  case 0x1F: cpu->A = impl_rr(cpu, cpu->A); cpu->F &= ~FLAG_Z; break;
  case 0x20: cpu_jump_rotation(cpu, cpu_read_imm8(cpu), (cpu->F & FLAG_Z) == 0);  break;
  case 0x21: cpu->HL = cpu_read_imm16(cpu); break;
  case 0x22: cpu_write_u8(cpu, cpu->HL++, cpu->A); break;
  case 0x23: cpu->HL++; bus_tick(cpu->bus); break;
  case 0x24: cpu->H = impl_inc_u8(cpu, cpu->H); break;
  case 0x25: cpu->H = impl_dec_u8(cpu, cpu->H); break;
  case 0x26: cpu->H = cpu_read_imm8(cpu); break;
  case 0x27: daa(cpu); break;
  case 0x28: cpu_jump_rotation(cpu, cpu_read_imm8(cpu), (cpu->F & FLAG_Z) != 0); break;
  case 0x29: impl_add_r16(cpu, cpu->HL); break;
  case 0x2A: cpu->A = cpu_read_u8(cpu, cpu->HL++); break;
  case 0x2B: cpu->HL--; bus_tick(cpu->bus); break;
  case 0x2C: cpu->L = impl_inc_u8(cpu, cpu->L); break;
  case 0x2D: cpu->L = impl_dec_u8(cpu, cpu->L); break;
  case 0x2E: cpu->L = cpu_read_imm8(cpu); break;
  case 0x2F: cpu->A = ~cpu->A; cpu->F |= FLAG_N | FLAG_H; break;
  case 0x30: cpu_jump_rotation(cpu, cpu_read_imm8(cpu), (cpu->F & FLAG_C) == 0); break;
  case 0x31: cpu->SP = cpu_read_imm16(cpu); break;
  case 0x32: cpu_write_u8(cpu, cpu->HL--, cpu->A); break;
  case 0x33: cpu->SP++; bus_tick(cpu->bus); break;
  case 0x34: cpu_write_u8(cpu, cpu->HL, impl_inc_u8(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x35: cpu_write_u8(cpu, cpu->HL, impl_dec_u8(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x36: cpu_write_u8(cpu, cpu->HL, cpu_read_imm8(cpu)); break;
  case 0x37: cpu->F &= ~(FLAG_N | FLAG_H); cpu->F |= FLAG_C; break;
  case 0x38: cpu_jump_rotation(cpu, cpu_read_imm8(cpu), (cpu->F & FLAG_C) != 0); break;
  case 0x39: impl_add_r16(cpu, cpu->SP); break;
  case 0x3A: cpu->A = cpu_read_u8(cpu, cpu->HL--); break;
  case 0x3B: cpu->SP--; bus_tick(cpu->bus); break;
  case 0x3C: cpu->A = impl_inc_u8(cpu, cpu->A); break;
  case 0x3D: cpu->A = impl_dec_u8(cpu, cpu->A); break;
  case 0x3E: cpu->A = cpu_read_imm8(cpu); break;
  case 0x3F: cpu->F &= ~(FLAG_N | FLAG_H); cpu->F ^= FLAG_C; break;
  case 0x40: /* cpu->B = cpu->B; */ break;
  case 0x41: cpu->B = cpu->C; break;
  case 0x42: cpu->B = cpu->D; break;
  case 0x43: cpu->B = cpu->E; break;
  case 0x44: cpu->B = cpu->H; break;
  case 0x45: cpu->B = cpu->L; break;
  case 0x46: cpu->B = cpu_read_u8(cpu, cpu->HL); break;
  case 0x47: cpu->B = cpu->A; break;
  case 0x48: cpu->C = cpu->B; break;
  case 0x49: /* cpu->C = cpu->C; */ break;
  case 0x4A: cpu->C = cpu->D; break;
  case 0x4B: cpu->C = cpu->E; break;
  case 0x4C: cpu->C = cpu->H; break;
  case 0x4D: cpu->C = cpu->L; break;
  case 0x4E: cpu->C = cpu_read_u8(cpu, cpu->HL); break;
  case 0x4F: cpu->C = cpu->A; break;
  case 0x50: cpu->D = cpu->B; break;
  case 0x51: cpu->D = cpu->C; break;
  case 0x52: /* cpu->D = cpu->D; */ break;
  case 0x53: cpu->D = cpu->E; break;
  case 0x54: cpu->D = cpu->H; break;
  case 0x55: cpu->D = cpu->L; break;
  case 0x56: cpu->D = cpu_read_u8(cpu, cpu->HL); break;
  case 0x57: cpu->D = cpu->A; break;
  case 0x58: cpu->E = cpu->B; break;
  case 0x59: cpu->E = cpu->C; break;
  case 0x5A: cpu->E = cpu->D; break;
  case 0x5B: /* cpu->E = cpu->E; */ break;
  case 0x5C: cpu->E = cpu->H; break;
  case 0x5D: cpu->E = cpu->L; break;
  case 0x5E: cpu->E = cpu_read_u8(cpu, cpu->HL); break;
  case 0x5F: cpu->E = cpu->A; break;
  case 0x60: cpu->H = cpu->B; break;
  case 0x61: cpu->H = cpu->C; break;
  case 0x62: cpu->H = cpu->D; break;
  case 0x63: cpu->H = cpu->E; break;
  case 0x64: /* cpu->H = cpu->H; */ break;
  case 0x65: cpu->H = cpu->L; break;
  case 0x66: cpu->H = cpu_read_u8(cpu, cpu->HL); break;
  case 0x67: cpu->H = cpu->A; break;
  case 0x68: cpu->L = cpu->B; break;
  case 0x69: cpu->L = cpu->C; break;
  case 0x6A: cpu->L = cpu->D; break;
  case 0x6B: cpu->L = cpu->E; break;
  case 0x6C: cpu->L = cpu->H; break;
  case 0x6D: /* cpu->L = cpu->L; */ break;
  case 0x6E: cpu->L = cpu_read_u8(cpu, cpu->HL); break;
  case 0x6F: cpu->L = cpu->A; break;
  case 0x70: cpu_write_u8(cpu, cpu->HL, cpu->B); break;
  case 0x71: cpu_write_u8(cpu, cpu->HL, cpu->C); break;
  case 0x72: cpu_write_u8(cpu, cpu->HL, cpu->D); break;
  case 0x73: cpu_write_u8(cpu, cpu->HL, cpu->E); break;
  case 0x74: cpu_write_u8(cpu, cpu->HL, cpu->H); break;
  case 0x75: cpu_write_u8(cpu, cpu->HL, cpu->L); break;
  case 0x76: halt(cpu); break;
  case 0x77: cpu_write_u8(cpu, cpu->HL, cpu->A); break;
  case 0x78: cpu->A = cpu->B; break;
  case 0x79: cpu->A = cpu->C; break;
  case 0x7A: cpu->A = cpu->D; break;
  case 0x7B: cpu->A = cpu->E; break;
  case 0x7C: cpu->A = cpu->H; break;
  case 0x7D: cpu->A = cpu->L; break;
  case 0x7E: cpu->A = cpu_read_u8(cpu, cpu->HL); break;
  case 0x7F: /* cpu->A = cpu->A; */ break;
  case 0x80: impl_add(cpu, cpu->B); break;
  case 0x81: impl_add(cpu, cpu->C); break;
  case 0x82: impl_add(cpu, cpu->D); break;
  case 0x83: impl_add(cpu, cpu->E); break;
  case 0x84: impl_add(cpu, cpu->H); break;
  case 0x85: impl_add(cpu, cpu->L); break;
  case 0x86: impl_add(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x87: impl_add(cpu, cpu->A); break;
  case 0x88: impl_adc(cpu, cpu->B); break;
  case 0x89: impl_adc(cpu, cpu->C); break;
  case 0x8A: impl_adc(cpu, cpu->D); break;
  case 0x8B: impl_adc(cpu, cpu->E); break;
  case 0x8C: impl_adc(cpu, cpu->H); break;
  case 0x8D: impl_adc(cpu, cpu->L); break;
  case 0x8E: impl_adc(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x8F: impl_adc(cpu, cpu->A); break;
  case 0x90: impl_sub(cpu, cpu->B); break;
  case 0x91: impl_sub(cpu, cpu->C); break;
  case 0x92: impl_sub(cpu, cpu->D); break;
  case 0x93: impl_sub(cpu, cpu->E); break;
  case 0x94: impl_sub(cpu, cpu->H); break;
  case 0x95: impl_sub(cpu, cpu->L); break;
  case 0x96: impl_sub(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x97: impl_sub(cpu, cpu->A); break;
  case 0x98: impl_sbc(cpu, cpu->B); break;
  case 0x99: impl_sbc(cpu, cpu->C); break;
  case 0x9A: impl_sbc(cpu, cpu->D); break;
  case 0x9B: impl_sbc(cpu, cpu->E); break;
  case 0x9C: impl_sbc(cpu, cpu->H); break;
  case 0x9D: impl_sbc(cpu, cpu->L); break;
  case 0x9E: impl_sbc(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x9F: impl_sbc(cpu, cpu->A); break;
  case 0xA0: impl_and(cpu, cpu->B); break;
  case 0xA1: impl_and(cpu, cpu->C); break;
  case 0xA2: impl_and(cpu, cpu->D); break;
  case 0xA3: impl_and(cpu, cpu->E); break;
  case 0xA4: impl_and(cpu, cpu->H); break;
  case 0xA5: impl_and(cpu, cpu->L); break;
  case 0xA6: impl_and(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0xA7: impl_and(cpu, cpu->A); break;
  case 0xA8: impl_xor(cpu, cpu->B); break;
  case 0xA9: impl_xor(cpu, cpu->C); break;
  case 0xAA: impl_xor(cpu, cpu->D); break;
  case 0xAB: impl_xor(cpu, cpu->E); break;
  case 0xAC: impl_xor(cpu, cpu->H); break;
  case 0xAD: impl_xor(cpu, cpu->L); break;
  case 0xAE: impl_xor(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0xAF: impl_xor(cpu, cpu->A); break;
  case 0xB0: impl_or(cpu, cpu->B); break;
  case 0xB1: impl_or(cpu, cpu->C); break;
  case 0xB2: impl_or(cpu, cpu->D); break;
  case 0xB3: impl_or(cpu, cpu->E); break;
  case 0xB4: impl_or(cpu, cpu->H); break;
  case 0xB5: impl_or(cpu, cpu->L); break;
  case 0xB6: impl_or(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0xB7: impl_or(cpu, cpu->A); break;
  case 0xB8: impl_cp(cpu, cpu->B); break;
  case 0xB9: impl_cp(cpu, cpu->C); break;
  case 0xBA: impl_cp(cpu, cpu->D); break;
  case 0xBB: impl_cp(cpu, cpu->E); break;
  case 0xBC: impl_cp(cpu, cpu->H); break;
  case 0xBD: impl_cp(cpu, cpu->L); break;
  case 0xBE: impl_cp(cpu, cpu_read_u8(cpu, cpu->HL)); break;
  case 0xBF: impl_cp(cpu, cpu->A); break;
  case 0xC0: bus_tick(cpu->bus); cpu_return(cpu, (cpu->F & FLAG_Z) == 0); break;
  case 0xC1: cpu->BC = cpu_pop_u16(cpu); break;
  case 0xC2: cpu_jump(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_Z) == 0); break;
  case 0xC3: cpu_jump(cpu, cpu_read_imm16(cpu), true); break;
  case 0xC4: cpu_call(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_Z) == 0); break;
  case 0xC5: bus_tick(cpu->bus); cpu_push_u16(cpu, cpu->BC); break;
  case 0xC6: impl_add(cpu, cpu_read_imm8(cpu)); break;
  case 0xC7: cpu_call(cpu, 0x00, true); break;
  case 0xC8: bus_tick(cpu->bus); cpu_return(cpu, (cpu->F & FLAG_Z) != 0); break;
  case 0xC9: cpu_return(cpu, true); break;
  case 0xCA: cpu_jump(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_Z) != 0); break;
  case 0xCB: cpu_execute_cb(cpu, cpu_read_imm8(cpu)); break;
  case 0xCC: cpu_call(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_Z) != 0); break;
  case 0xCD: cpu_call(cpu, cpu_read_imm16(cpu), true); break;
  case 0xCE: impl_adc(cpu, cpu_read_imm8(cpu)); break;
  case 0xCF: cpu_call(cpu, 0x08, true); break;
  case 0xD0: bus_tick(cpu->bus); cpu_return(cpu, (cpu->F & FLAG_C) == 0); break;
  case 0xD1: cpu->DE = cpu_pop_u16(cpu); break;
  case 0xD2: cpu_jump(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_C) == 0); break;
  case 0xD3: cpu_illegal(cpu, opcode); break;
  case 0xD4: cpu_call(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_C) == 0); break;
  case 0xD5: bus_tick(cpu->bus); cpu_push_u16(cpu, cpu->DE); break;
  case 0xD6: impl_sub(cpu, cpu_read_imm8(cpu)); break;
  case 0xD7: cpu_call(cpu, 0x10, true); break;
  case 0xD8: bus_tick(cpu->bus); cpu_return(cpu, (cpu->F & FLAG_C) != 0); break;
  case 0xD9: cpu_return(cpu, cpu->ime_pending = true); break;
  case 0xDA: cpu_jump(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_C) != 0); break;
  case 0xDB: cpu_illegal(cpu, opcode); break;
  case 0xDC: cpu_call(cpu, cpu_read_imm16(cpu), (cpu->F & FLAG_C) == 0); break;
  case 0xDD: cpu_illegal(cpu, opcode); break;
  case 0xDE: impl_sbc(cpu, cpu_read_imm8(cpu)); break;
  case 0xDF: cpu_call(cpu, 0x18, true); break;
  case 0xE0: cpu_write_u8(cpu, 0xFF00 + cpu_read_imm8(cpu), cpu->A); break;
  case 0xE1: cpu->HL = cpu_pop_u16(cpu); break;
  case 0xE2: cpu_write_u8(cpu, 0xFF00 + cpu->C, cpu->A); break;
  case 0xE3: cpu_illegal(cpu, opcode); break;
  case 0xE4: cpu_illegal(cpu, opcode); break;
  case 0xE5: bus_tick(cpu->bus); cpu_push_u16(cpu, cpu->HL); break;
  case 0xE6: impl_and(cpu, cpu_read_imm8(cpu)); break;
  case 0xE7: cpu_call(cpu, 0x20, true); break;
  case 0xE8: cpu->SP = impl_add_sp_e8(cpu, cpu_read_imm8(cpu)); bus_tick(cpu->bus); break;
  case 0xE9: cpu->PC = cpu->HL; break;
  case 0xEA: cpu_write_u8(cpu, cpu_read_imm16(cpu), cpu->A); break;
  case 0xEB: cpu_illegal(cpu, opcode); break;
  case 0xEC: cpu_illegal(cpu, opcode); break;
  case 0xED: cpu_illegal(cpu, opcode); break;
  case 0xEE: impl_xor(cpu, cpu_read_imm8(cpu)); break;
  case 0xEF: cpu_call(cpu, 0x28, true); break;
  case 0xF0: cpu->A = cpu_read_u8(cpu, 0xFF00 + cpu_read_imm8(cpu)); break;
  case 0xF1: cpu->AF = cpu_pop_u16(cpu) & 0xFFF0; break;
  case 0xF2: cpu->A = cpu_read_u8(cpu, 0xFF00 + cpu->C); break;
  case 0xF3: cpu->IME = cpu->ime_pending = false; break;
  case 0xF4: cpu_illegal(cpu, opcode); break;
  case 0xF5: bus_tick(cpu->bus); cpu_push_u16(cpu, cpu->AF); break;
  case 0xF6: impl_or(cpu, cpu_read_imm8(cpu)); break;
  case 0xF7: cpu_call(cpu, 0x30, true); break;
  case 0xF8: cpu->HL = impl_add_sp_e8(cpu, cpu_read_imm8(cpu)); break;
  case 0xF9: cpu->SP = cpu->HL; bus_tick(cpu->bus); break;
  case 0xFA: cpu->A = cpu_read_u8(cpu, cpu_read_imm16(cpu)); break;
  case 0xFB: cpu->ime_pending = true; break;
  case 0xFC: cpu_illegal(cpu, opcode); break;
  case 0xFD: cpu_illegal(cpu, opcode); break;
  case 0xFE: impl_cp(cpu, cpu_read_imm8(cpu)); break;
  case 0xFF: cpu_call(cpu, 0x38, true); break;
    // clang-format on
  }
}

void cpu_execute_cb(struct cpu *cpu, uint8_t opcode) {
  switch (opcode) {
    // clang-format off
  case 0x00: cpu->B = impl_rlc(cpu, cpu->B); break;
  case 0x01: cpu->C = impl_rlc(cpu, cpu->C); break;
  case 0x02: cpu->D = impl_rlc(cpu, cpu->D); break;
  case 0x03: cpu->E = impl_rlc(cpu, cpu->E); break;
  case 0x04: cpu->H = impl_rlc(cpu, cpu->H); break;
  case 0x05: cpu->L = impl_rlc(cpu, cpu->L); break;
  case 0x06: cpu_write_u8(cpu, cpu->HL, impl_rlc(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x07: cpu->A = impl_rlc(cpu, cpu->A); break;
  case 0x08: cpu->B = impl_rrc(cpu, cpu->B); break;
  case 0x09: cpu->C = impl_rrc(cpu, cpu->C); break;
  case 0x0A: cpu->D = impl_rrc(cpu, cpu->D); break;
  case 0x0B: cpu->E = impl_rrc(cpu, cpu->E); break;
  case 0x0C: cpu->H = impl_rrc(cpu, cpu->H); break;
  case 0x0D: cpu->L = impl_rrc(cpu, cpu->L); break;
  case 0x0E: cpu_write_u8(cpu, cpu->HL, impl_rrc(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x0F: cpu->A = impl_rrc(cpu, cpu->A); break;
  case 0x10: cpu->B = impl_rl(cpu, cpu->B); break;
  case 0x11: cpu->C = impl_rl(cpu, cpu->C); break;
  case 0x12: cpu->D = impl_rl(cpu, cpu->D); break;
  case 0x13: cpu->E = impl_rl(cpu, cpu->E); break;
  case 0x14: cpu->H = impl_rl(cpu, cpu->H); break;
  case 0x15: cpu->L = impl_rl(cpu, cpu->L); break;
  case 0x16: cpu_write_u8(cpu, cpu->HL, impl_rl(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x17: cpu->A = impl_rl(cpu, cpu->A); break;
  case 0x18: cpu->B = impl_rr(cpu, cpu->B); break;
  case 0x19: cpu->C = impl_rr(cpu, cpu->C); break;
  case 0x1A: cpu->D = impl_rr(cpu, cpu->D); break;
  case 0x1B: cpu->E = impl_rr(cpu, cpu->E); break;
  case 0x1C: cpu->H = impl_rr(cpu, cpu->H); break;
  case 0x1D: cpu->L = impl_rr(cpu, cpu->L); break;
  case 0x1E: cpu_write_u8(cpu, cpu->HL, impl_rr(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x1F: cpu->A = impl_rr(cpu, cpu->A); break;
  case 0x20: cpu->B = impl_sla(cpu, cpu->B); break;
  case 0x21: cpu->C = impl_sla(cpu, cpu->C); break;
  case 0x22: cpu->D = impl_sla(cpu, cpu->D); break;
  case 0x23: cpu->E = impl_sla(cpu, cpu->E); break;
  case 0x24: cpu->H = impl_sla(cpu, cpu->H); break;
  case 0x25: cpu->L = impl_sla(cpu, cpu->L); break;
  case 0x26: cpu_write_u8(cpu, cpu->HL, impl_sla(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x27: cpu->A = impl_sla(cpu, cpu->A); break;
  case 0x28: cpu->B = impl_sra(cpu, cpu->B); break;
  case 0x29: cpu->C = impl_sra(cpu, cpu->C); break;
  case 0x2A: cpu->D = impl_sra(cpu, cpu->D); break;
  case 0x2B: cpu->E = impl_sra(cpu, cpu->E); break;
  case 0x2C: cpu->H = impl_sra(cpu, cpu->H); break;
  case 0x2D: cpu->L = impl_sra(cpu, cpu->L); break;
  case 0x2E: cpu_write_u8(cpu, cpu->HL, impl_sra(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x2F: cpu->A = impl_sra(cpu, cpu->A); break;
  case 0x30: cpu->B = impl_swap(cpu, cpu->B); break;
  case 0x31: cpu->C = impl_swap(cpu, cpu->C); break;
  case 0x32: cpu->D = impl_swap(cpu, cpu->D); break;
  case 0x33: cpu->E = impl_swap(cpu, cpu->E); break;
  case 0x34: cpu->H = impl_swap(cpu, cpu->H); break;
  case 0x35: cpu->L = impl_swap(cpu, cpu->L); break;
  case 0x36: cpu_write_u8(cpu, cpu->HL, impl_swap(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x37: cpu->A = impl_swap(cpu, cpu->A); break;
  case 0x38: cpu->B = impl_srl(cpu, cpu->B); break;
  case 0x39: cpu->C = impl_srl(cpu, cpu->C); break;
  case 0x3A: cpu->D = impl_srl(cpu, cpu->D); break;
  case 0x3B: cpu->E = impl_srl(cpu, cpu->E); break;
  case 0x3C: cpu->H = impl_srl(cpu, cpu->H); break;
  case 0x3D: cpu->L = impl_srl(cpu, cpu->L); break;
  case 0x3E: cpu_write_u8(cpu, cpu->HL, impl_srl(cpu, cpu_read_u8(cpu, cpu->HL))); break;
  case 0x3F: cpu->A = impl_srl(cpu, cpu->A); break;
  case 0x40: impl_bit_b3(cpu, 0, cpu->B); break;
  case 0x41: impl_bit_b3(cpu, 0, cpu->C); break;
  case 0x42: impl_bit_b3(cpu, 0, cpu->D); break;
  case 0x43: impl_bit_b3(cpu, 0, cpu->E); break;
  case 0x44: impl_bit_b3(cpu, 0, cpu->H); break;
  case 0x45: impl_bit_b3(cpu, 0, cpu->L); break;
  case 0x46: impl_bit_b3(cpu, 0, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x47: impl_bit_b3(cpu, 0, cpu->A); break;
  case 0x48: impl_bit_b3(cpu, 1, cpu->B); break;
  case 0x49: impl_bit_b3(cpu, 1, cpu->C); break;
  case 0x4A: impl_bit_b3(cpu, 1, cpu->D); break;
  case 0x4B: impl_bit_b3(cpu, 1, cpu->E); break;
  case 0x4C: impl_bit_b3(cpu, 1, cpu->H); break;
  case 0x4D: impl_bit_b3(cpu, 1, cpu->L); break;
  case 0x4E: impl_bit_b3(cpu, 1, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x4F: impl_bit_b3(cpu, 1, cpu->A); break;
  case 0x50: impl_bit_b3(cpu, 2, cpu->B); break;
  case 0x51: impl_bit_b3(cpu, 2, cpu->C); break;
  case 0x52: impl_bit_b3(cpu, 2, cpu->D); break;
  case 0x53: impl_bit_b3(cpu, 2, cpu->E); break;
  case 0x54: impl_bit_b3(cpu, 2, cpu->H); break;
  case 0x55: impl_bit_b3(cpu, 2, cpu->L); break;
  case 0x56: impl_bit_b3(cpu, 2, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x57: impl_bit_b3(cpu, 2, cpu->A); break;
  case 0x58: impl_bit_b3(cpu, 3, cpu->B); break;
  case 0x59: impl_bit_b3(cpu, 3, cpu->C); break;
  case 0x5A: impl_bit_b3(cpu, 3, cpu->D); break;
  case 0x5B: impl_bit_b3(cpu, 3, cpu->E); break;
  case 0x5C: impl_bit_b3(cpu, 3, cpu->H); break;
  case 0x5D: impl_bit_b3(cpu, 3, cpu->L); break;
  case 0x5E: impl_bit_b3(cpu, 3, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x5F: impl_bit_b3(cpu, 3, cpu->A); break;
  case 0x60: impl_bit_b3(cpu, 4, cpu->B); break;
  case 0x61: impl_bit_b3(cpu, 4, cpu->C); break;
  case 0x62: impl_bit_b3(cpu, 4, cpu->D); break;
  case 0x63: impl_bit_b3(cpu, 4, cpu->E); break;
  case 0x64: impl_bit_b3(cpu, 4, cpu->H); break;
  case 0x65: impl_bit_b3(cpu, 4, cpu->L); break;
  case 0x66: impl_bit_b3(cpu, 4, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x67: impl_bit_b3(cpu, 4, cpu->A); break;
  case 0x68: impl_bit_b3(cpu, 5, cpu->B); break;
  case 0x69: impl_bit_b3(cpu, 5, cpu->C); break;
  case 0x6A: impl_bit_b3(cpu, 5, cpu->D); break;
  case 0x6B: impl_bit_b3(cpu, 5, cpu->E); break;
  case 0x6C: impl_bit_b3(cpu, 5, cpu->H); break;
  case 0x6D: impl_bit_b3(cpu, 5, cpu->L); break;
  case 0x6E: impl_bit_b3(cpu, 5, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x6F: impl_bit_b3(cpu, 5, cpu->A); break;
  case 0x70: impl_bit_b3(cpu, 6, cpu->B); break;
  case 0x71: impl_bit_b3(cpu, 6, cpu->C); break;
  case 0x72: impl_bit_b3(cpu, 6, cpu->D); break;
  case 0x73: impl_bit_b3(cpu, 6, cpu->E); break;
  case 0x74: impl_bit_b3(cpu, 6, cpu->H); break;
  case 0x75: impl_bit_b3(cpu, 6, cpu->L); break;
  case 0x76: impl_bit_b3(cpu, 6, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x77: impl_bit_b3(cpu, 6, cpu->A); break;
  case 0x78: impl_bit_b3(cpu, 7, cpu->B); break;
  case 0x79: impl_bit_b3(cpu, 7, cpu->C); break;
  case 0x7A: impl_bit_b3(cpu, 7, cpu->D); break;
  case 0x7B: impl_bit_b3(cpu, 7, cpu->E); break;
  case 0x7C: impl_bit_b3(cpu, 7, cpu->H); break;
  case 0x7D: impl_bit_b3(cpu, 7, cpu->L); break;
  case 0x7E: impl_bit_b3(cpu, 7, cpu_read_u8(cpu, cpu->HL)); break;
  case 0x7F: impl_bit_b3(cpu, 7, cpu->A); break;
  case 0x80: cpu->B &= ~(1 << 0); break;
  case 0x81: cpu->C &= ~(1 << 0); break;
  case 0x82: cpu->D &= ~(1 << 0); break;
  case 0x83: cpu->E &= ~(1 << 0); break;
  case 0x84: cpu->H &= ~(1 << 0); break;
  case 0x85: cpu->L &= ~(1 << 0); break;
  case 0x86: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 0)); break;
  case 0x87: cpu->A &= ~(1 << 0); break;
  case 0x88: cpu->B &= ~(1 << 1); break;
  case 0x89: cpu->C &= ~(1 << 1); break;
  case 0x8A: cpu->D &= ~(1 << 1); break;
  case 0x8B: cpu->E &= ~(1 << 1); break;
  case 0x8C: cpu->H &= ~(1 << 1); break;
  case 0x8D: cpu->L &= ~(1 << 1); break;
  case 0x8E: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 1)); break;
  case 0x8F: cpu->A &= ~(1 << 1); break;
  case 0x90: cpu->B &= ~(1 << 2); break;
  case 0x91: cpu->C &= ~(1 << 2); break;
  case 0x92: cpu->D &= ~(1 << 2); break;
  case 0x93: cpu->E &= ~(1 << 2); break;
  case 0x94: cpu->H &= ~(1 << 2); break;
  case 0x95: cpu->L &= ~(1 << 2); break;
  case 0x96: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 2)); break;
  case 0x97: cpu->A &= ~(1 << 2); break;
  case 0x98: cpu->B &= ~(1 << 3); break;
  case 0x99: cpu->C &= ~(1 << 3); break;
  case 0x9A: cpu->D &= ~(1 << 3); break;
  case 0x9B: cpu->E &= ~(1 << 3); break;
  case 0x9C: cpu->H &= ~(1 << 3); break;
  case 0x9D: cpu->L &= ~(1 << 3); break;
  case 0x9E: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 3)); break;
  case 0x9F: cpu->A &= ~(1 << 3); break;
  case 0xA0: cpu->B &= ~(1 << 4); break;
  case 0xA1: cpu->C &= ~(1 << 4); break;
  case 0xA2: cpu->D &= ~(1 << 4); break;
  case 0xA3: cpu->E &= ~(1 << 4); break;
  case 0xA4: cpu->H &= ~(1 << 4); break;
  case 0xA5: cpu->L &= ~(1 << 4); break;
  case 0xA6: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 4)); break;
  case 0xA7: cpu->A &= ~(1 << 4); break;
  case 0xA8: cpu->B &= ~(1 << 5); break;
  case 0xA9: cpu->C &= ~(1 << 5); break;
  case 0xAA: cpu->D &= ~(1 << 5); break;
  case 0xAB: cpu->E &= ~(1 << 5); break;
  case 0xAC: cpu->H &= ~(1 << 5); break;
  case 0xAD: cpu->L &= ~(1 << 5); break;
  case 0xAE: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 5)); break;
  case 0xAF: cpu->A &= ~(1 << 5); break;
  case 0xB0: cpu->B &= ~(1 << 6); break;
  case 0xB1: cpu->C &= ~(1 << 6); break;
  case 0xB2: cpu->D &= ~(1 << 6); break;
  case 0xB3: cpu->E &= ~(1 << 6); break;
  case 0xB4: cpu->H &= ~(1 << 6); break;
  case 0xB5: cpu->L &= ~(1 << 6); break;
  case 0xB6: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 6)); break;
  case 0xB7: cpu->A &= ~(1 << 6); break;
  case 0xB8: cpu->B &= ~(1 << 7); break;
  case 0xB9: cpu->C &= ~(1 << 7); break;
  case 0xBA: cpu->D &= ~(1 << 7); break;
  case 0xBB: cpu->E &= ~(1 << 7); break;
  case 0xBC: cpu->H &= ~(1 << 7); break;
  case 0xBD: cpu->L &= ~(1 << 7); break;
  case 0xBE: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) & ~(1 << 7)); break;
  case 0xBF: cpu->A &= ~(1 << 7); break;
  case 0xC0: cpu->B |= 1 << 0; break;
  case 0xC1: cpu->C |= 1 << 0; break;
  case 0xC2: cpu->D |= 1 << 0; break;
  case 0xC3: cpu->E |= 1 << 0; break;
  case 0xC4: cpu->H |= 1 << 0; break;
  case 0xC5: cpu->L |= 1 << 0; break;
  case 0xC6: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 0); break;
  case 0xC7: cpu->A |= 1 << 0; break;
  case 0xC8: cpu->B |= 1 << 1; break;
  case 0xC9: cpu->C |= 1 << 1; break;
  case 0xCA: cpu->D |= 1 << 1; break;
  case 0xCB: cpu->E |= 1 << 1; break;
  case 0xCC: cpu->H |= 1 << 1; break;
  case 0xCD: cpu->L |= 1 << 1; break;
  case 0xCE: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 1); break;
  case 0xCF: cpu->A |= 1 << 1; break;
  case 0xD0: cpu->B |= 1 << 2; break;
  case 0xD1: cpu->C |= 1 << 2; break;
  case 0xD2: cpu->D |= 1 << 2; break;
  case 0xD3: cpu->E |= 1 << 2; break;
  case 0xD4: cpu->H |= 1 << 2; break;
  case 0xD5: cpu->L |= 1 << 2; break;
  case 0xD6: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 2); break;
  case 0xD7: cpu->A |= 1 << 2; break;
  case 0xD8: cpu->B |= 1 << 3; break;
  case 0xD9: cpu->C |= 1 << 3; break;
  case 0xDA: cpu->D |= 1 << 3; break;
  case 0xDB: cpu->E |= 1 << 3; break;
  case 0xDC: cpu->H |= 1 << 3; break;
  case 0xDD: cpu->L |= 1 << 3; break;
  case 0xDE: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 3); break;
  case 0xDF: cpu->A |= 1 << 3; break;
  case 0xE0: cpu->B |= 1 << 4; break;
  case 0xE1: cpu->C |= 1 << 4; break;
  case 0xE2: cpu->D |= 1 << 4; break;
  case 0xE3: cpu->E |= 1 << 4; break;
  case 0xE4: cpu->H |= 1 << 4; break;
  case 0xE5: cpu->L |= 1 << 4; break;
  case 0xE6: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 4); break;
  case 0xE7: cpu->A |= 1 << 4; break;
  case 0xE8: cpu->B |= 1 << 5; break;
  case 0xE9: cpu->C |= 1 << 5; break;
  case 0xEA: cpu->D |= 1 << 5; break;
  case 0xEB: cpu->E |= 1 << 5; break;
  case 0xEC: cpu->H |= 1 << 5; break;
  case 0xED: cpu->L |= 1 << 5; break;
  case 0xEE: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 5); break;
  case 0xEF: cpu->A |= 1 << 5; break;
  case 0xF0: cpu->B |= 1 << 6; break;
  case 0xF1: cpu->C |= 1 << 6; break;
  case 0xF2: cpu->D |= 1 << 6; break;
  case 0xF3: cpu->E |= 1 << 6; break;
  case 0xF4: cpu->H |= 1 << 6; break;
  case 0xF5: cpu->L |= 1 << 6; break;
  case 0xF6: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 6); break;
  case 0xF7: cpu->A |= 1 << 6; break;
  case 0xF8: cpu->B |= 1 << 7; break;
  case 0xF9: cpu->C |= 1 << 7; break;
  case 0xFA: cpu->D |= 1 << 7; break;
  case 0xFB: cpu->E |= 1 << 7; break;
  case 0xFC: cpu->H |= 1 << 7; break;
  case 0xFD: cpu->L |= 1 << 7; break;
  case 0xFE: cpu_write_u8(cpu, cpu->HL, cpu_read_u8(cpu, cpu->HL) | 1 << 7); break;
  case 0xFF: cpu->A |= 1 << 7; break;
    // clang-format on
  }
}

void cpu_set_flag_as(struct cpu *cpu, enum cpu_flags flag, bool val) {
  cpu->F = val ? cpu->F | flag : cpu->F & ~flag;
}

// Memory operations

uint8_t cpu_read_u8(const struct cpu *cpu, uint16_t addr) {
  const uint8_t ret = bus_read_byte(cpu->bus, addr);
  bus_tick(cpu->bus);
  return ret;
}

uint16_t cpu_read_u16(const struct cpu *cpu, uint16_t addr) {
  const uint8_t lo = cpu_read_u8(cpu, addr);
  const uint8_t hi = cpu_read_u8(cpu, addr + 1);
  return (uint16_t)hi << 8 | lo;
}

uint8_t cpu_read_imm8(struct cpu *cpu) { return cpu_read_u8(cpu, cpu->PC++); }

uint16_t cpu_read_imm16(struct cpu *cpu) {
  const uint16_t u16 = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  return u16;
}

void cpu_write_u8(const struct cpu *cpu, uint16_t addr, uint8_t val) {
  bus_write_byte(cpu->bus, addr, val);
  bus_tick(cpu->bus);
}

void cpu_write_u16(const struct cpu *cpu, uint16_t addr, uint16_t val) {
  cpu_write_u8(cpu, addr, val & 0xFF);
  cpu_write_u8(cpu, addr + 1, val >> 8);
}

void cpu_push_u16(struct cpu *cpu, uint16_t u16) {
  cpu->SP -= 2;
  cpu_write_u16(cpu, cpu->SP, u16);
}

uint16_t cpu_pop_u16(struct cpu *cpu) {
  const uint16_t u16 = cpu_read_u16(cpu, cpu->SP);
  cpu->SP += 2;
  return u16;
}

void cpu_jump(struct cpu *cpu, uint16_t addr, bool cond) {
  if (cond) {
    cpu->PC = addr;
    bus_tick(cpu->bus);
  }
}

void cpu_jump_rotation(struct cpu *cpu, int8_t offset, bool cond) {
  if (cond) {
    cpu->PC += offset;
    bus_tick(cpu->bus);
  }
}

void cpu_call(struct cpu *cpu, uint16_t addr, bool cond) {
  if (cond) {
    cpu_push_u16(cpu, cpu->PC);
    cpu->PC = addr;
    bus_tick(cpu->bus);
  }
}

void cpu_return(struct cpu *cpu, bool cond) {
  if (cond) {
    cpu->PC = cpu_pop_u16(cpu);
    bus_tick(cpu->bus);
  }
}
