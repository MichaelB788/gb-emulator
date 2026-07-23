#include "cpu_instrs.h"
#include "bitwise.h"
#include "bus.h"
#include "cpu.h"
#include "impl_cpu_instrs.h"
#include "optables.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define Y_BIT_FIELD(opcode) ((opcode >> 3) & 0x7)
#define Z_BIT_FIELD(opcode) (opcode & 0x7)

/// Load instructions

uint8_t ld_r8_r8(struct cpu *cpu) {
  cpu->r8[Y_BIT_FIELD(cpu->opcode)] = cpu->r8[Z_BIT_FIELD(cpu->opcode)];
  return 4;
}

uint8_t ld_r8_n8(struct cpu *cpu) {
  cpu->r8[Y_BIT_FIELD(cpu->opcode)] = cpu_fetch_n8(cpu);
  return 8;
}

uint8_t ld_r16_n16(struct cpu *cpu) {
  cpu_set_r16(cpu, cpu_fetch_n16(cpu));
  return 12;
}

uint8_t ld_hl_ind_r8(struct cpu *cpu) {
  cpu_write_hl(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 8;
}

uint8_t ld_hl_ind_n8(struct cpu *cpu) {
  cpu_write_hl(cpu, cpu_fetch_n8(cpu));
  return 12;
}

uint8_t ld_r8_hl_ind(struct cpu *cpu) {
  cpu->r8[Y_BIT_FIELD(cpu->opcode)] = cpu_read_hl(cpu);
  return 8;
}

uint8_t ld_r16_ind_a(struct cpu *cpu) {
  bus_write_byte(cpu->bus, cpu_get_r16mem(cpu), cpu->A);
  return 8;
}

uint8_t ld_n16_ind_a(struct cpu *cpu) {
  bus_write_byte(cpu->bus, cpu_fetch_n16(cpu), cpu->A);
  return 16;
}

uint8_t ldh_n8_ind_a(struct cpu *cpu) {
  bus_write_byte(cpu->bus, (0xFF00 | cpu_fetch_n8(cpu)), cpu->A);
  return 12;
}

uint8_t ldh_c_ind_a(struct cpu *cpu) {
  bus_write_byte(cpu->bus, (0xFF00 | cpu->C), cpu->A);
  return 8;
}

uint8_t ld_a_r16_ind(struct cpu *cpu) {
  cpu->A = bus_read_byte(cpu->bus, cpu_get_r16mem(cpu));
  return 8;
}

uint8_t ld_a_n16_ind(struct cpu *cpu) {
  cpu->A = bus_read_byte(cpu->bus, cpu_fetch_n16(cpu));
  return 16;
}

uint8_t ldh_a_n8_ind(struct cpu *cpu) {
  cpu->A = bus_read_byte(cpu->bus, (0xFF00 | cpu_fetch_n8(cpu)));
  return 12;
}

uint8_t ldh_a_c_ind(struct cpu *cpu) {
  cpu->A = bus_read_byte(cpu->bus, (0xFF00 | cpu->C));
  return 8;
}

/// 8-bit arithmetic instructions

uint8_t adc_r8(struct cpu *cpu) {
  impl_adc(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t adc_hl_ind(struct cpu *cpu) {
  impl_adc(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t adc_n8(struct cpu *cpu) {
  impl_adc(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t add_r8(struct cpu *cpu) {
  impl_add(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t add_hl_ind(struct cpu *cpu) {
  impl_add(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t add_n8(struct cpu *cpu) {
  impl_add(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t sbc_r8(struct cpu *cpu) {
  impl_sbc(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t sbc_hl_ind(struct cpu *cpu) {
  impl_sbc(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t sbc_n8(struct cpu *cpu) {
  impl_sbc(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t sub_r8(struct cpu *cpu) {
  impl_sub(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t sub_hl_ind(struct cpu *cpu) {
  impl_sub(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t sub_n8(struct cpu *cpu) {
  impl_sub(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t cp_r8(struct cpu *cpu) {
  impl_cp(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t cp_hl_ind(struct cpu *cpu) {
  impl_cp(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t cp_n8(struct cpu *cpu) {
  impl_cp(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t inc_r8(struct cpu *cpu) {
  const uint8_t i = Y_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_inc_u8(cpu, cpu->r8[i]);
  return 4;
}

uint8_t inc_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_inc_u8(cpu, cpu_read_hl(cpu)));
  return 12;
}

uint8_t dec_r8(struct cpu *cpu) {
  const uint8_t i = Y_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_dec_u8(cpu, cpu->r8[i]);
  return 4;
}

uint8_t dec_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_dec_u8(cpu, cpu_read_hl(cpu)));
  return 12;
}

uint8_t add_hl_r16(struct cpu *cpu) {
  impl_add_r16(cpu, cpu_get_r16(cpu));
  return 8;
}

uint8_t inc_r16(struct cpu *cpu) {
  cpu_set_r16(cpu, cpu_get_r16(cpu) + 1);
  return 8;
}

uint8_t dec_r16(struct cpu *cpu) {
  cpu_set_r16(cpu, cpu_get_r16(cpu) - 1);
  return 8;
}

/// Bitwise logic instructions

uint8_t and_r8(struct cpu *cpu) {
  impl_and(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t and_hl_ind(struct cpu *cpu) {
  impl_and(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t and_n8(struct cpu *cpu) {
  impl_and(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t or_r8(struct cpu *cpu) {
  impl_or(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t or_hl_ind(struct cpu *cpu) {
  impl_or(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t or_n8(struct cpu *cpu) {
  impl_or(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t xor_r8(struct cpu *cpu) {
  impl_xor(cpu, cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 4;
}

uint8_t xor_hl_ind(struct cpu *cpu) {
  impl_xor(cpu, cpu_read_hl(cpu));
  return 8;
}

uint8_t xor_n8(struct cpu *cpu) {
  impl_xor(cpu, cpu_fetch_n8(cpu));
  return 8;
}

uint8_t cpl(struct cpu *cpu) {
  cpu->A = ~(cpu->A);
  write_bit(&cpu->F, FLAG_N, true);
  write_bit(&cpu->F, FLAG_H, true);
  return 4;
}

/// Bit flag instructions

uint8_t bit_b3_r8(struct cpu *cpu) {
  impl_bit_b3(cpu, Y_BIT_FIELD(cpu->opcode), cpu->r8[Z_BIT_FIELD(cpu->opcode)]);
  return 8;
}

uint8_t bit_b3_hl_ind(struct cpu *cpu) {
  impl_bit_b3(cpu, Y_BIT_FIELD(cpu->opcode), cpu_read_hl(cpu));
  return 12;
}

uint8_t res_b3_r8(struct cpu *cpu) {
  clear_bit(&cpu->r8[Z_BIT_FIELD(cpu->opcode)], Y_BIT_FIELD(cpu->opcode));
  return 8;
}

uint8_t res_b3_hl_ind(struct cpu *cpu) {
  uint8_t hl_mem = cpu_read_hl(cpu);
  clear_bit(&hl_mem, Y_BIT_FIELD(cpu->opcode));
  cpu_write_hl(cpu, hl_mem);
  return 16;
}

uint8_t set_b3_r8(struct cpu *cpu) {
  set_bit(&cpu->r8[Z_BIT_FIELD(cpu->opcode)], Y_BIT_FIELD(cpu->opcode));
  return 8;
}

uint8_t set_b3_hl_ind(struct cpu *cpu) {
  uint8_t hl_mem = cpu_read_hl(cpu);
  set_bit(&hl_mem, Y_BIT_FIELD(cpu->opcode));
  cpu_write_hl(cpu, hl_mem);
  return 16;
}

/// Bit shfit instructions

uint8_t rl_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_rl(cpu, cpu->r8[i]);
  return 8;
}

uint8_t rl_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_rl(cpu, cpu_read_hl(cpu)));
  return 16;
}

uint8_t rla(struct cpu *cpu) {
  cpu->A = impl_rl(cpu, cpu->A);
  clear_bit(&cpu->F, FLAG_Z);
  return 4;
}

uint8_t rlc_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_rlc(cpu, cpu->r8[i]);
  return 8;
}

uint8_t rlc_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_rlc(cpu, cpu_read_hl(cpu)));
  return 16;
}

uint8_t rlca(struct cpu *cpu) {
  cpu->A = impl_rlc(cpu, cpu->A);
  clear_bit(&cpu->F, FLAG_Z);
  return 4;
}

uint8_t rr_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_rr(cpu, cpu->r8[i]);
  return 8;
}

uint8_t rr_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_rr(cpu, cpu_read_hl(cpu)));
  return 16;
}

uint8_t rra(struct cpu *cpu) {
  cpu->A = impl_rr(cpu, cpu->A);
  clear_bit(&cpu->F, FLAG_Z);
  return 4;
}

uint8_t rrc_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_rrc(cpu, cpu->r8[i]);
  return 8;
}

uint8_t rrc_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_rrc(cpu, cpu_read_hl(cpu)));
  return 16;
}

uint8_t rrca(struct cpu *cpu) {
  cpu->A = impl_rrc(cpu, cpu->A);
  clear_bit(&cpu->F, FLAG_Z);
  return 4;
}

uint8_t sla_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_sla(cpu, cpu->r8[i]);
  return 8;
}

uint8_t sla_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_sla(cpu, cpu_read_hl(cpu)));
  return 16;
}

uint8_t sra_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_sra(cpu, cpu->r8[i]);
  return 8;
}

uint8_t sra_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_sra(cpu, cpu_read_hl(cpu)));
  return 16;
}

uint8_t srl_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_srl(cpu, cpu->r8[i]);
  return 8;
}

uint8_t srl_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_srl(cpu, cpu_read_hl(cpu)));
  return 16;
}

uint8_t swap_r8(struct cpu *cpu) {
  const uint8_t i = Z_BIT_FIELD(cpu->opcode);
  cpu->r8[i] = impl_swap(cpu, cpu->r8[i]);
  return 8;
}

uint8_t swap_hl_ind(struct cpu *cpu) {
  cpu_write_hl(cpu, impl_swap(cpu, cpu_read_hl(cpu)));
  return 16;
}

/// Jumps and subroutine instructions

uint8_t call_n16(struct cpu *cpu) {
  const uint16_t jmp_addr = cpu_fetch_n16(cpu);
  cpu_push_n16(cpu, cpu->PC);
  cpu->PC = jmp_addr;
  return 24;
}

uint8_t call_cc_n16(struct cpu *cpu) {
  const uint16_t jmp_addr = cpu_fetch_n16(cpu);
  if (cpu_test_cond(cpu)) {
    cpu_push_n16(cpu, cpu->PC);
    cpu->PC = jmp_addr;
    return 24;
  }
  return 12;
}

uint8_t jp_hl(struct cpu *cpu) {
  cpu->PC = cpu_get_hl(cpu);
  return 4;
}

uint8_t jp_n16(struct cpu *cpu) {
  cpu->PC = cpu_fetch_n16(cpu);
  return 16;
}

uint8_t jp_cc_n16(struct cpu *cpu) {
  const uint16_t jmp_addr = cpu_fetch_n16(cpu);
  if (cpu_test_cond(cpu)) {
    cpu->PC = jmp_addr;
    return 16;
  }
  return 12;
}

uint8_t jr_e8(struct cpu *cpu) {
  cpu->PC += (int8_t)cpu_fetch_n8(cpu);
  return 12;
}

uint8_t jr_cc_e8(struct cpu *cpu) {
  const int8_t offset = (int8_t)cpu_fetch_n8(cpu);
  if (cpu_test_cond(cpu)) {
    cpu->PC += offset;
    return 12;
  }
  return 8;
}

uint8_t ret_cc(struct cpu *cpu) {
  if (cpu_test_cond(cpu)) {
    cpu->PC = cpu_pop_n16(cpu);
    return 20;
  }
  return 8;
}

uint8_t ret(struct cpu *cpu) {
  cpu->PC = cpu_pop_n16(cpu);
  return 20;
}

uint8_t reti(struct cpu *cpu) {
  cpu->PC = cpu_pop_n16(cpu);
  cpu->ime_pending = true;
  return 20;
}

uint8_t rst_vec(struct cpu *cpu) {
  const uint16_t jmp_addr = cpu->opcode & 0x38; // This mask isolates the y
                                                // field without shifting it.
  cpu_push_n16(cpu, cpu->PC);
  cpu->PC = jmp_addr;
  return 16;
}

/// Carry flag instructions

uint8_t ccf(struct cpu *cpu) {
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  toggle_bit(&cpu->F, FLAG_C);
  return 4;
}

uint8_t scf(struct cpu *cpu) {
  clear_bit(&cpu->F, FLAG_N);
  clear_bit(&cpu->F, FLAG_H);
  set_bit(&cpu->F, FLAG_C);
  return 4;
}

/// Stack manipulation instructions

uint8_t add_sp_e8(struct cpu *cpu) {
  cpu->SP = impl_add_sp_e8(cpu, (int8_t)cpu_fetch_n8(cpu));
  return 16;
}

uint8_t ld_hl_sp_e8(struct cpu *cpu) {
  cpu_set_hl(cpu, impl_add_sp_e8(cpu, (int8_t)cpu_fetch_n8(cpu)));
  return 12;
}

uint8_t ld_n16_ind_sp(struct cpu *cpu) {
  const uint16_t n16 = cpu_fetch_n16(cpu);
  bus_write_byte(cpu->bus, n16, cpu->SP & 0xFF);
  bus_write_byte(cpu->bus, n16 + 1, cpu->SP >> 8);
  return 20;
}

uint8_t ld_sp_hl(struct cpu *cpu) {
  cpu->SP = cpu_get_hl(cpu);
  return 8;
}

uint8_t pop_r16stk(struct cpu *cpu) {
  cpu_set_r16stk(cpu, cpu_pop_n16(cpu));
  return 12;
}

uint8_t push_r16stk(struct cpu *cpu) {
  cpu_push_n16(cpu, cpu_get_r16stk(cpu));
  return 16;
}

/// Interrupt-related instructions

uint8_t di(struct cpu *cpu) {
  cpu->IME = false;
  return 4;
}

uint8_t ei(struct cpu *cpu) {
  cpu->ime_pending = true;
  return 4;
}

uint8_t halt(struct cpu *cpu) {
  cpu->state = CPU_HALTED;
  return 4;
}

/// Misc.

uint8_t daa(struct cpu *cpu) {
  const uint8_t A = cpu->A, F = cpu->F;
  uint8_t result = 0, adjustment = 0;

  if (is_bit_set(F, FLAG_N)) {
    if (is_bit_set(F, FLAG_H)) {
      adjustment |= 0x6;
    }
    if (is_bit_set(F, FLAG_C)) {
      adjustment |= 0x60;
    }
    result = A - adjustment;
  } else {
    if (is_bit_set(F, FLAG_H) || (A & 0xF) > 0x9) {
      adjustment |= 0x6;
    }
    if (is_bit_set(F, FLAG_C) || A > 0x99) {
      adjustment |= 0x60;
      write_bit(&cpu->F, FLAG_C, true);
    }
    result = A + adjustment;
  }

  write_bit(&cpu->F, FLAG_Z, result == 0);
  write_bit(&cpu->F, FLAG_H, false);

  cpu->A = result;
  return 4;
}

uint8_t nop(struct cpu *cpu) { return 4; }

uint8_t stop(struct cpu *cpu) {
  cpu_fetch_n8(cpu);
  cpu->state = CPU_STOPPED;
  cpu->bus->timer.system_counter = 0;
  cpu->bus->timer.divider = 0;
  return 4;
}

uint8_t prefix(struct cpu *cpu) {
  cpu->opcode = cpu_fetch_n8(cpu);
  return cbprefixed_ins[cpu->opcode](cpu);
}

uint8_t illegal(struct cpu *cpu) {
  fprintf(stderr, "Illegal instruction: 0x%02X\n", cpu->opcode);
  return 0;
}
