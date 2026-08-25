#include "cpu_instrs.h"
#include "bus.h"
#include "cpu.h"
#include "impl_cpu_instrs.h"
#include "interrupts.h"
#include <stdint.h>
#include <stdio.h>

#define OPCODE_Y 0x38
#define OPCODE_Z 0x7

#define REG8_Y(opcode) ((opcode & OPCODE_Y) >> 3)
#define REG8_Z(opcode) (opcode & OPCODE_Z)

#define BIT3_Y(opcode) ((opcode & OPCODE_Y) >> 3)

/// Load instructions

void ld_r8_r8(struct cpu *cpu) {
  cpu->r8[REG8_Y(cpu->IR)] = cpu->r8[REG8_Z(cpu->IR)];
}

void ld_r8_u8(struct cpu *cpu) {
  cpu->r8[REG8_Y(cpu->IR)] = cpu_read_byte(cpu, cpu->PC++);
}

void ld_r16_u16(struct cpu *cpu) {
  const uint16_t val16 = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_set_r16(cpu, val16);
}

void ld_hl_ind_r8(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu), cpu->r8[REG8_Z(cpu->IR)]);
}

void ld_hl_ind_u8(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu), cpu_read_byte(cpu, cpu->PC++));
}

void ld_r8_hl_ind(struct cpu *cpu) {
  cpu->r8[REG8_Y(cpu->IR)] = cpu_read_byte(cpu, cpu_get_hl(cpu));
}

void ld_r16_ind_a(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_r16mem(cpu), cpu->A);
}

void ld_u16_ind_a(struct cpu *cpu) {
  const uint16_t addr = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_write_byte(cpu, addr, cpu->A);
}

void ldh_u8_ind_a(struct cpu *cpu) {
  cpu_write_byte(cpu, (0xFF00 | cpu_read_byte(cpu, cpu->PC++)), cpu->A);
}

void ldh_c_ind_a(struct cpu *cpu) {
  cpu_write_byte(cpu, (0xFF00 | cpu->C), cpu->A);
}

void ld_a_r16_ind(struct cpu *cpu) {
  cpu->A = cpu_read_byte(cpu, cpu_get_r16mem(cpu));
}

void ld_a_u16_ind(struct cpu *cpu) {
  const uint16_t addr = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu->A = cpu_read_byte(cpu, addr);
}

void ldh_a_u8_ind(struct cpu *cpu) {
  cpu->A = cpu_read_byte(cpu, (0xFF00 | cpu_read_byte(cpu, cpu->PC++)));
}

void ldh_a_c_ind(struct cpu *cpu) {
  cpu->A = cpu_read_byte(cpu, (0xFF00 | cpu->C));
}

/// 8-bit arithmetic instructions

void adc_r8(struct cpu *cpu) { impl_adc(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void adc_hl_ind(struct cpu *cpu) {
  impl_adc(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void adc_u8(struct cpu *cpu) { impl_adc(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void add_r8(struct cpu *cpu) { impl_add(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void add_hl_ind(struct cpu *cpu) {
  impl_add(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void add_u8(struct cpu *cpu) { impl_add(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void sbc_r8(struct cpu *cpu) { impl_sbc(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void sbc_hl_ind(struct cpu *cpu) {
  impl_sbc(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void sbc_u8(struct cpu *cpu) { impl_sbc(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void sub_r8(struct cpu *cpu) { impl_sub(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void sub_hl_ind(struct cpu *cpu) {
  impl_sub(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void sub_u8(struct cpu *cpu) { impl_sub(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void cp_r8(struct cpu *cpu) { impl_cp(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void cp_hl_ind(struct cpu *cpu) {
  impl_cp(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void cp_u8(struct cpu *cpu) { impl_cp(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void inc_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Y(cpu->IR);
  cpu->r8[i] = impl_inc_u8(cpu, cpu->r8[i]);
}

void inc_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_inc_u8(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void dec_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Y(cpu->IR);
  cpu->r8[i] = impl_dec_u8(cpu, cpu->r8[i]);
}

void dec_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_dec_u8(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void add_hl_r16(struct cpu *cpu) { impl_add_r16(cpu, cpu_get_r16(cpu)); }

void inc_r16(struct cpu *cpu) {
  cpu_set_r16(cpu, cpu_get_r16(cpu) + 1);
  bus_tick(cpu->bus); // Setting r16 consumes a machine cycle
}

void dec_r16(struct cpu *cpu) {
  cpu_set_r16(cpu, cpu_get_r16(cpu) - 1);
  bus_tick(cpu->bus); // Setting r16 consumes a machine cycle
}

/// Bitwise logic instructions

void and_r8(struct cpu *cpu) { impl_and(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void and_hl_ind(struct cpu *cpu) {
  impl_and(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void and_u8(struct cpu *cpu) { impl_and(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void or_r8(struct cpu *cpu) { impl_or(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void or_hl_ind(struct cpu *cpu) {
  impl_or(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void or_u8(struct cpu *cpu) { impl_or(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void xor_r8(struct cpu *cpu) { impl_xor(cpu, cpu->r8[REG8_Z(cpu->IR)]); }

void xor_hl_ind(struct cpu *cpu) {
  impl_xor(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void xor_u8(struct cpu *cpu) { impl_xor(cpu, cpu_read_byte(cpu, cpu->PC++)); }

void cpl(struct cpu *cpu) {
  cpu->A = ~(cpu->A);
  cpu->F |= (FLAG_N | FLAG_H);
}

/// Bit flag instructions

void bit_b3_r8(struct cpu *cpu) {
  impl_bit_b3(cpu, BIT3_Y(cpu->IR), cpu->r8[REG8_Z(cpu->IR)]);
}

void bit_b3_hl_ind(struct cpu *cpu) {
  impl_bit_b3(cpu, BIT3_Y(cpu->IR), cpu_read_byte(cpu, cpu_get_hl(cpu)));
}

void res_b3_r8(struct cpu *cpu) {
  cpu->r8[REG8_Z(cpu->IR)] &= ~(1 << BIT3_Y(cpu->IR));
}

void res_b3_hl_ind(struct cpu *cpu) {
  const uint8_t result =
      cpu_read_byte(cpu, cpu_get_hl(cpu)) & ~(1 << BIT3_Y(cpu->IR));
  cpu_write_byte(cpu, cpu_get_hl(cpu), result);
}

void set_b3_r8(struct cpu *cpu) {
  cpu->r8[REG8_Z(cpu->IR)] |= 1 << BIT3_Y(cpu->IR);
}

void set_b3_hl_ind(struct cpu *cpu) {
  const uint8_t result =
      cpu_read_byte(cpu, cpu_get_hl(cpu)) | (1 << BIT3_Y(cpu->IR));
  cpu_write_byte(cpu, cpu_get_hl(cpu), result);
}

/// Bit shfit instructions

void rl_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_rl(cpu, cpu->r8[i]);
}

void rl_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_rl(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void rla(struct cpu *cpu) {
  cpu->A = impl_rl(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void rlc_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_rlc(cpu, cpu->r8[i]);
}

void rlc_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_rlc(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void rlca(struct cpu *cpu) {
  cpu->A = impl_rlc(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void rr_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_rr(cpu, cpu->r8[i]);
}

void rr_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_rr(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void rra(struct cpu *cpu) {
  cpu->A = impl_rr(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void rrc_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_rrc(cpu, cpu->r8[i]);
}

void rrc_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_rrc(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void rrca(struct cpu *cpu) {
  cpu->A = impl_rrc(cpu, cpu->A);
  cpu->F &= ~FLAG_Z;
}

void sla_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_sla(cpu, cpu->r8[i]);
}

void sla_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_sla(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void sra_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_sra(cpu, cpu->r8[i]);
}

void sra_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_sra(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void srl_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_srl(cpu, cpu->r8[i]);
}

void srl_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_srl(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

void swap_r8(struct cpu *cpu) {
  const uint8_t i = REG8_Z(cpu->IR);
  cpu->r8[i] = impl_swap(cpu, cpu->r8[i]);
}

void swap_hl_ind(struct cpu *cpu) {
  cpu_write_byte(cpu, cpu_get_hl(cpu),
                 impl_swap(cpu, cpu_read_byte(cpu, cpu_get_hl(cpu))));
}

/// Jumps and subroutine instructions

void call_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_call_a16(cpu, addr, true);
}

void call_cc_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_call_a16(cpu, addr, cpu_test_cond(cpu));
}

void jp_hl(struct cpu *cpu) { cpu->PC = cpu_get_hl(cpu); }

void jp_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_jump_a16(cpu, addr, true);
}

void jp_cc_a16(struct cpu *cpu) {
  const uint16_t addr = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_jump_a16(cpu, addr, cpu_test_cond(cpu));
}

void jr_e8(struct cpu *cpu) {
  const int8_t offset = (int8_t)cpu_read_byte(cpu, cpu->PC++);
  cpu_jump_a16(cpu, cpu->PC + offset, true);
}

void jr_cc_e8(struct cpu *cpu) {
  const int8_t offset = (int8_t)cpu_read_byte(cpu, cpu->PC++);
  cpu_jump_a16(cpu, cpu->PC + offset, cpu_test_cond(cpu));
}

void ret_cc(struct cpu *cpu) {
  bus_tick(cpu->bus); // Internal cycle before test
  cpu_return(cpu, cpu_test_cond(cpu));
}

void ret(struct cpu *cpu) { cpu_return(cpu, true); }

void reti(struct cpu *cpu) {
  cpu_return(cpu, true);
  cpu->ei_called = true;
}

void rst_vec(struct cpu *cpu) { cpu_call_a16(cpu, cpu->IR & OPCODE_Y, true); }

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
  cpu->SP = impl_add_sp_e8(cpu);
  bus_tick(cpu->bus); // Internal tick, likely when setting SP
}

void ld_hl_sp_e8(struct cpu *cpu) { cpu_set_hl(cpu, impl_add_sp_e8(cpu)); }

void ld_u16_ind_sp(struct cpu *cpu) {
  const uint16_t addr = cpu_read_word(cpu, cpu->PC);
  cpu->PC += 2;
  cpu_write_word(cpu, addr, cpu->SP);
}

void ld_sp_hl(struct cpu *cpu) {
  cpu->SP = cpu_get_hl(cpu);
  bus_tick(cpu->bus); // Internal tick, likely when setting SP
}

void pop_r16stk(struct cpu *cpu) {
  const uint16_t u16 = cpu_read_word(cpu, cpu->SP);
  cpu->SP += 2;
  cpu_set_r16stk(cpu, u16);
}

void push_r16stk(struct cpu *cpu) {
  bus_tick(cpu->bus); // Internal tick
  cpu->SP -= 2;
  cpu_write_word(cpu, cpu->SP, cpu_get_r16stk(cpu));
}

/// Interrupt-related instructions

void di(struct cpu *cpu) {
  cpu->IME = false;
  cpu->ei_called = false;
}

void ei(struct cpu *cpu) { cpu->ei_called = true; }

void halt(struct cpu *cpu) {
  if (!cpu->IME && interrupts_pending(cpu->bus->interrupts) != 0) {
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
  cpu_read_byte(cpu, cpu->PC++);
  cpu->state = CPU_STOPPED;
  cpu->bus->timer.system_counter = 0;
  cpu->bus->timer.DIV = 0;
}

void prefix(struct cpu *cpu) { cpu->executing_cb_op = true; }

void illegal(struct cpu *cpu) {
  fprintf(stderr, "Illegal instruction: 0x%02X\n", cpu->IR);
}
