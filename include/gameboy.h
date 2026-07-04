#pragma once
#include "byte_sizes.h"
#include "cartridge.h"
#include "cpu.h"
#include "interrupts.h"
#include "serial_transfer.h"
#include "timer.h"
#include <stdint.h>
#include <stdio.h>

enum gameboy_state { GB_RUNNING, GB_STOPPED, GB_HALTED };

struct gameboy {
  enum gameboy_state state;
  uint8_t opcode;
  uint8_t joypad;

  struct cpu cpu;
  struct interrupts interrupt;
  struct serial_transfer serial;
  struct timer timer;
  struct cartridge cartridge;

  uint8_t vram[KiB_8];
  uint8_t wram[KiB_8];
  uint8_t hram[127];
};

bool init_gameboy(struct gameboy *gb, const char *path_to_rom);

void close_gameboy(struct gameboy *gb);

void run_gameboy_loop(struct gameboy *gb);

uint8_t bus_read(struct gameboy *gb, uint16_t addr);

void bus_write(struct gameboy *gb, uint16_t addr, uint8_t val);

uint8_t io_read(struct gameboy *gb, uint16_t addr);

void io_write(struct gameboy *gb, uint16_t addr, uint8_t val);

int service_interrupts(struct gameboy *gb);

void log_curr_instr(struct gameboy *gb, FILE *output);
