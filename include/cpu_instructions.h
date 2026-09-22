#pragma once
#include <stdint.h>

struct cpu;

void cpu_add_u8(struct cpu *cpu, uint8_t u8);
void cpu_adc_u8(struct cpu *cpu, uint8_t u8);
void cpu_sub_u8(struct cpu *cpu, uint8_t u8);
void cpu_sbc_u8(struct cpu *cpu, uint8_t u8);
void cpu_cp_u8(struct cpu *cpu, uint8_t u8);

[[nodiscard]] uint8_t cpu_inc_u8(struct cpu *cpu, uint8_t u8);
[[nodiscard]] uint8_t cpu_dec_u8(struct cpu *cpu, uint8_t u8);

void cpu_add_r16(struct cpu *cpu, uint16_t u16); // M-cycles: 1

// M-cycles: 1 if setting HL, 2 if setting SP
[[nodiscard]] uint16_t cpu_add_sp_i8(struct cpu *cpu, int8_t e8);

void cpu_and_u8(struct cpu *cpu, uint8_t u8);
void cpu_xor_u8(struct cpu *cpu, uint8_t u8);
void cpu_or_u8(struct cpu *cpu, uint8_t u8);

void cpu_bit_b3_u8(struct cpu *cpu, uint8_t b3, uint8_t u8);

[[nodiscard]] uint8_t cpu_rl_u8(struct cpu *cpu, uint8_t u8);
[[nodiscard]] uint8_t cpu_rlc_u8(struct cpu *cpu, uint8_t u8);

[[nodiscard]] uint8_t cpu_rr_u8(struct cpu *cpu, uint8_t u8);
[[nodiscard]] uint8_t cpu_rrc_u8(struct cpu *cpu, uint8_t u8);

[[nodiscard]] uint8_t cpu_sla_u8(struct cpu *cpu, uint8_t u8);
[[nodiscard]] uint8_t cpu_sra_u8(struct cpu *cpu, uint8_t u8);
[[nodiscard]] uint8_t cpu_srl_u8(struct cpu *cpu, uint8_t u8);

[[nodiscard]] uint8_t cpu_swap_u8(struct cpu *cpu, uint8_t u8);

void cpu_daa(struct cpu *cpu);
void cpu_halt(struct cpu *cpu);
void cpu_illegal(struct cpu *cpu, uint8_t opcode);
