#pragma once
#include "byte_sizes.h"
#include "cartridge.h"
#include "cpu.h"
#include "io_registers.h"
#include <stdint.h>

typedef enum { GB_RUNNING, GB_STOPPED, GB_HALTED } gb_state_t;

struct gameboy {
  gb_state_t state;
  uint8_t instruction_register;
  bool interrupt_enable;
  bool path_taken;

  struct cpu cpu;
  struct io_registers io;

  uint8_t vram[KiB_8];
  uint8_t wram[KiB_8];
  uint8_t hram[127];

  struct cartridge *cartridge;
};

bool init_gameboy(struct gameboy *gb, const char *path_to_rom);

void close_gameboy(struct gameboy *gb);

uint8_t read_byte(struct gameboy *gb, uint16_t addr);

void write_byte(struct gameboy *gb, uint16_t addr, uint8_t val);

void execute_instruction(struct gameboy *gb, uint8_t opcode);

void execute_cb_instruction(struct gameboy *gb, uint8_t opcode);
