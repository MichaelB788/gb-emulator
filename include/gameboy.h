#pragma once
#include "byte_sizes.h"
#include "cartridge.h"
#include "cpu.h"
#include "interrupts.h"
#include "serial_transfer.h"
#include "timer.h"
#include <stdint.h>

struct gameboy {
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

uint8_t bus_read(struct gameboy *gb, uint16_t addr);

void bus_write(struct gameboy *gb, uint16_t addr, uint8_t val);

uint8_t io_read(struct gameboy *gb, uint16_t addr);

void io_write(struct gameboy *gb, uint16_t addr, uint8_t val);

int handle_interrupts(struct gameboy *gb);
