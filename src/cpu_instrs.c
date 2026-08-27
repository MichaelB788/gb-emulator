#include "cpu_instrs.h"
#include "bus.h"
#include "cpu.h"
#include "impl_cpu_instrs.h"
#include "interrupts.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

/// Load instructions

void ld_r8_r8(struct cpu *cpu) { cpu_set_r8_y(cpu, cpu_get_r8_z(cpu)); }

void ld_r8_u8(struct cpu *cpu) {
  cpu_set_r8_y(cpu, cpu_read_u8(cpu, cpu->PC++));
}

void ld_r16_u16(struct cpu *cpu) {
  const uint16_t val16 = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_set_r16(cpu, val16);
}

void ld_r16_ind_a(struct cpu *cpu) {
  cpu_write_u8(cpu, cpu_get_r16mem(cpu), cpu->A);
}

void ld_u16_ind_a(struct cpu *cpu) {
  const uint16_t addr = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_write_u8(cpu, addr, cpu->A);
}

void ldh_u8_ind_a(struct cpu *cpu) {
  cpu_write_u8(cpu, (0xFF00 | cpu_read_u8(cpu, cpu->PC++)), cpu->A);
}

void ldh_c_ind_a(struct cpu *cpu) {
  cpu_write_u8(cpu, (0xFF00 | cpu->C), cpu->A);
}

void ld_a_r16_ind(struct cpu *cpu) {
  cpu->A = cpu_read_u8(cpu, cpu_get_r16mem(cpu));
}

void ld_a_u16_ind(struct cpu *cpu) {
  const uint16_t addr = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu->A = cpu_read_u8(cpu, addr);
}

void ldh_a_u8_ind(struct cpu *cpu) {
  cpu->A = cpu_read_u8(cpu, (0xFF00 | cpu_read_u8(cpu, cpu->PC++)));
}

void ldh_a_c_ind(struct cpu *cpu) {
  cpu->A = cpu_read_u8(cpu, (0xFF00 | cpu->C));
}

/// 8-bit arithmetic instructions

void adc_r8(struct cpu *cpu) { impl_adc(cpu, cpu_get_r8_z(cpu)); }
void adc_u8(struct cpu *cpu) { impl_adc(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void add_r8(struct cpu *cpu) { impl_add(cpu, cpu_get_r8_z(cpu)); }
void add_u8(struct cpu *cpu) { impl_add(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void sbc_r8(struct cpu *cpu) { impl_sbc(cpu, cpu_get_r8_z(cpu)); }
void sbc_u8(struct cpu *cpu) { impl_sbc(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void sub_r8(struct cpu *cpu) { impl_sub(cpu, cpu_get_r8_z(cpu)); }
void sub_u8(struct cpu *cpu) { impl_sub(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void cp_r8(struct cpu *cpu) { impl_cp(cpu, cpu_get_r8_z(cpu)); }
void cp_u8(struct cpu *cpu) { impl_cp(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void inc_r8(struct cpu *cpu) {
  cpu_set_r8_y(cpu, impl_inc_u8(cpu, cpu_get_r8_y(cpu)));
}

void dec_r8(struct cpu *cpu) {
  cpu_set_r8_y(cpu, impl_dec_u8(cpu, cpu_get_r8_y(cpu)));
}

void add_hl_r16(struct cpu *cpu) {
  impl_add_r16(cpu, cpu_get_r16(cpu));
  bus_tick(cpu->bus); // Internal
}

void inc_r16(struct cpu *cpu) {
  cpu_set_r16(cpu, cpu_get_r16(cpu) + 1);
  bus_tick(cpu->bus); // Internal
}

void dec_r16(struct cpu *cpu) {
  cpu_set_r16(cpu, cpu_get_r16(cpu) - 1);
  bus_tick(cpu->bus); // Internal
}

/// Bitwise logic instructions

void and_r8(struct cpu *cpu) { impl_and(cpu, cpu_get_r8_z(cpu)); }
void and_u8(struct cpu *cpu) { impl_and(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void or_r8(struct cpu *cpu) { impl_or(cpu, cpu_get_r8_z(cpu)); }
void or_u8(struct cpu *cpu) { impl_or(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void xor_r8(struct cpu *cpu) { impl_xor(cpu, cpu_get_r8_z(cpu)); }
void xor_u8(struct cpu *cpu) { impl_xor(cpu, cpu_read_u8(cpu, cpu->PC++)); }

void cpl(struct cpu *cpu) {
  cpu->A = ~(cpu->A);
  cpu->F |= (FLAG_N | FLAG_H);
}

/// Bit flag instructions

// Extracts the 3-bit index from the opcode
[[nodiscard]] static uint8_t cpu_b3(uint8_t op) { return op >> 3 & 0x7; }

void bit_b3_r8(struct cpu *cpu) {
  impl_bit_b3(cpu, cpu_b3(cpu->IR), cpu_get_r8_z(cpu));
}

void res_b3_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, cpu_get_r8_z(cpu) & ~(1 << cpu_b3(cpu->IR)));
}

void set_b3_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, cpu_get_r8_z(cpu) | 1 << cpu_b3(cpu->IR));
}

/// Bit shfit instructions

void rl_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_rl(cpu, cpu_get_r8_z(cpu)));
}

void rla(struct cpu *cpu) {
  cpu->A = impl_rl(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void rlc_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_rlc(cpu, cpu_get_r8_z(cpu)));
}

void rlca(struct cpu *cpu) {
  cpu->A = impl_rlc(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void rr_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_rr(cpu, cpu_get_r8_z(cpu)));
}

void rra(struct cpu *cpu) {
  cpu->A = impl_rr(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void rrc_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_rrc(cpu, cpu_get_r8_z(cpu)));
}

void rrca(struct cpu *cpu) {
  cpu->A = impl_rrc(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void sla_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_sla(cpu, cpu_get_r8_z(cpu)));
}

void sra_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_sra(cpu, cpu_get_r8_z(cpu)));
}

void srl_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_srl(cpu, cpu_get_r8_z(cpu)));
}

void swap_r8(struct cpu *cpu) {
  cpu_set_r8_z(cpu, impl_swap(cpu, cpu_get_r8_z(cpu)));
}

/// Jumps and subroutine instructions

void call_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_call(cpu, addr, true);
}

void call_cc_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_call(cpu, addr, cpu_cc(cpu));
}

void jp_hl(struct cpu *cpu) { cpu->PC = cpu->HL; }

void jp_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_jump(cpu, addr, true);
}

void jp_cc_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_jump(cpu, addr, cpu_cc(cpu));
}

void jr_e8(struct cpu *cpu) {
  cpu_jump_rotation(cpu, cpu_read_u8(cpu, cpu->PC++), true);
}

void jr_cc_e8(struct cpu *cpu) {
  cpu_jump_rotation(cpu, cpu_read_u8(cpu, cpu->PC++), cpu_cc(cpu));
}

void ret_cc(struct cpu *cpu) {
  bus_tick(cpu->bus); // Internal
  cpu_return(cpu, cpu_cc(cpu));
}

void ret(struct cpu *cpu) { cpu_return(cpu, true); }

void reti(struct cpu *cpu) {
  cpu_return(cpu, true);
  cpu->ime_pending = true;
}

void rst_vec(struct cpu *cpu) { cpu_call(cpu, cpu->IR & 0x38, true); }

/// Carry flag instructions

void ccf(struct cpu *cpu) {
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu->F ^= FLAG_C;
}

void scf(struct cpu *cpu) {
  cpu->F &= ~(FLAG_N | FLAG_H);
  cpu->F |= FLAG_C;
}

/// Stack manipulation instructions

void add_sp_e8(struct cpu *cpu) {
  const uint16_t result =
      impl_add_sp_e8(cpu, bus_read_byte(cpu->bus, cpu->PC++));
  bus_tick(cpu->bus); // Internal
  cpu->SP = result;
  bus_tick(cpu->bus); // Internal
}

void ld_hl_sp_e8(struct cpu *cpu) {
  cpu->HL = impl_add_sp_e8(cpu, bus_read_byte(cpu->bus, cpu->PC++));
  bus_tick(cpu->bus); // Internal
}

void ld_u16_ind_sp(struct cpu *cpu) {
  const uint16_t addr = cpu_read_u16(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_write_u16(cpu, addr, cpu->SP);
}

void ld_sp_hl(struct cpu *cpu) {
  cpu->SP = cpu->HL;
  bus_tick(cpu->bus); // Internal
}

void pop_r16stk(struct cpu *cpu) {
  const uint16_t u16 = cpu_read_u16(cpu, cpu->SP);
  cpu->SP += 2;
  cpu_set_r16stk(cpu, u16);
}

void push_r16stk(struct cpu *cpu) {
  bus_tick(cpu->bus); // Internal
  cpu->SP -= 2;
  cpu_write_u16(cpu, cpu->SP, cpu_get_r16stk(cpu));
}

/// Interrupt-related instructions

void di(struct cpu *cpu) { cpu->IME = cpu->ime_pending = false; }

void ei(struct cpu *cpu) { cpu->ime_pending = true; }

void halt(struct cpu *cpu) {
  const struct interrupts *in = &cpu->bus->interrupts;
  if (!cpu->IME && (in->IF & in->IE) != 0) {
    cpu->halt_bug = true;
  } else {
    cpu->state = CPU_HALTED;
  }
}

/// Misc.

void daa(struct cpu *cpu) {
  const uint8_t A = cpu->A, F = cpu->F;
  uint8_t result = 0, adjustment = 0;

  if ((F & FLAG_N) != 0) {
    if ((F & FLAG_H) != 0) {
      adjustment |= 0x6;
    }
    if ((F & FLAG_C) != 0) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    if ((F & FLAG_H) != 0 || (A & 0xF) > 0x9) {
      adjustment |= 0x6;
    }
    if ((F & FLAG_C) != 0 || A > 0x99) {
      adjustment |= 0x60;
      cpu->F |= FLAG_C;
    }
    result = A + adjustment;
  }

  cpu_write_flags(cpu, FLAG_Z, result == 0);
  cpu->F &= ~FLAG_H;

  cpu->A = result;
}

void nop(struct cpu *cpu) {}

void stop(struct cpu *cpu) {
  cpu->state = CPU_STOPPED;
  cpu->bus->timer.system_counter = cpu->bus->timer.DIV = 0;
}

void prefix(struct cpu *cpu) { cpu->executing_cb_op = true; }

void illegal(struct cpu *cpu) {
  fprintf(stderr, "Illegal instruction: 0x%02X\n", cpu->IR);
}
