#pragma once
#include "cartridge.h"
#include "interrupt.h"
#include "ppu.h"
#include "serial_transfer.h"
#include "timer.h"
#include <stdint.h>

struct bus {
  uint8_t joypad; // 0xFF00: JOYP / P1
  struct serial_transfer serial_transfer;
  struct interrupt interrupt;
  struct timer timer;
  struct ppu ppu;

  uint8_t wram[8 * 1024]; // 8 KiB work ram, [0xC000, 0xDFFF]
  uint8_t hram[127];      // high ram, [0xFF80, 0xFFFE]

  struct cartridge *cartridge;
};

void bus_tick(struct bus *bus); // Advances by 1 M-cycle / 4 T-cycles

uint8_t bus_read(const struct bus *bus, uint16_t a16);

void bus_write(struct bus *bus, uint16_t a16, uint8_t u8);
