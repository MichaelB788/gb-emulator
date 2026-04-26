#pragma once
#include "core/bus.h"
#include <stdbool.h>
#include <stdint.h>

/**
 * Values set for each flag correspond to their bit index. For more info, see:
 * https://gbdev.io/pandocs/CPU_Registers_and_Flags.html#the-flags-register-lower-8-bits-of-af-register
 */
typedef enum { FLAG_Z = 7, FLAG_N = 6, FLAG_H = 5, FLAG_C = 4 } Flag;

typedef enum { CPU_RUNNING, CPU_STOPPED, CPU_HALTED } CPU_State;

typedef struct {
  union {
    struct {
      uint8_t C, B;
    };
    uint16_t BC;
  };
  union {
    struct {
      uint8_t E, D;
    };
    uint16_t DE;
  };
  union {
    struct {
      uint8_t L, H;
    };
    uint16_t HL;
  };
  union {
    struct {
      uint8_t F, A;
    };
    uint16_t AF;
  };

  uint16_t PC, SP;
  uint8_t opcode, cycles_taken;
  bool IME;
  CPU_State state;

  uint8_t *r8[8];   // B, C, D, E, H, L, F, A
  uint16_t *r16[4]; // BC, DE, HL, SP
  Bus *bus;
} CPU;

typedef struct {
  const char *name;
  uint8_t cycles;
  void (*exec)(CPU *);
} Instruction;

// Links the CPU to the bus and initializes pointer to member arrays.
void init_cpu(CPU *cpu, Bus *bus);

// Fetches, decodes, and executes the next instruction in memory
uint8_t step(CPU *cpu);

// Logs the current instruction and CPU register state
void log_ins(const CPU *cpu, const Instruction *ins);

// Obtains the y field of the opcode bit pattern [xx yyy zzz].
uint8_t op_y(uint8_t op);

// Obtains the z field of the opcode bit pattern [xx yyy zzz].
uint8_t op_z(uint8_t op);

/**
 * Returns a pointer to an 8-bit CPU register based on the current opcode.
 *
 * Note: Attempting to access what should be [HL] is undefined behavior.
 */
uint8_t *r8(CPU *cpu);

/**
 * Returns a pointer to an 8-bit CPU register based on the current opcode.
 * This function should only be used in register-to-register loads.
 *
 * Note: Attempting to access what should be [HL] is undefined behavior.
 */
uint8_t *r8_dest(CPU *cpu);

/**
 * Obtains a pointer to a 16-bit CPU register based on the current opcode.
 * This includes: [BC, DE, HL, SP]
 */
uint16_t *r16(CPU *cpu);

// Returns true if the condition code for the given CPU state is satisfied
bool check_cc(const CPU *cpu);

// Reads and returns the next immediate 8-bit operand from memory, advancing PC
uint8_t read_n8(CPU *cpu);

// Reads and returns the 8-bit value at the memory address pointed to by HL
uint8_t read_hl(const CPU *cpu);

// Reads and returns the next immediate 16-bit operand from memory, advancing PC
uint16_t read_n16(CPU *cpu);

// Writes an 8-bit value to the memory address pointed to by HL
void write_hl(CPU *cpu, uint8_t val);

// Sets the specified CPU flag to the given boolean value
void set_flag(CPU *cpu, Flag flag, bool val);

// Returns the current boolean value of the specified CPU flag
bool get_flag(const CPU *cpu, Flag flag);
