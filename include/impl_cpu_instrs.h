#pragma once
#include <stdint.h>

struct cpu;

/// 8-bit arithmetic implementations
void impl_add(struct cpu *cpu, uint8_t operand);
void impl_adc(struct cpu *cpu, uint8_t operand);

void impl_sub(struct cpu *cpu, uint8_t operand);
void impl_sbc(struct cpu *cpu, uint8_t operand);

void impl_cp(struct cpu *cpu, uint8_t operand);

[[nodiscard]] uint8_t impl_inc_u8(struct cpu *cpu, uint8_t operand);
[[nodiscard]] uint8_t impl_dec_u8(struct cpu *cpu, uint8_t operand);

/// 16-bit arithmetic implementations

// M-cycles: 1
void impl_add_r16(struct cpu *cpu, uint16_t operand);

// M-cycles: 1 if setting HL, 2 if setting SP
[[nodiscard]] uint16_t impl_add_sp_e8(struct cpu *cpu, int8_t e8);

/// Bitwise logic implementations
void impl_and(struct cpu *cpu, uint8_t operand);
void impl_xor(struct cpu *cpu, uint8_t operand);
void impl_or(struct cpu *cpu, uint8_t operand);

/// Bit flag implementations
void impl_bit_b3(struct cpu *cpu, uint8_t b3, uint8_t r8);

/// Bit-shift implementations
[[nodiscard]] uint8_t impl_rl(struct cpu *cpu, uint8_t operand);
[[nodiscard]] uint8_t impl_rlc(struct cpu *cpu, uint8_t operand);

[[nodiscard]] uint8_t impl_rr(struct cpu *cpu, uint8_t operand);
[[nodiscard]] uint8_t impl_rrc(struct cpu *cpu, uint8_t operand);

[[nodiscard]] uint8_t impl_sla(struct cpu *cpu, uint8_t operand);
[[nodiscard]] uint8_t impl_sra(struct cpu *cpu, uint8_t operand);
[[nodiscard]] uint8_t impl_srl(struct cpu *cpu, uint8_t operand);

[[nodiscard]] uint8_t impl_swap(struct cpu *cpu, uint8_t operand);
