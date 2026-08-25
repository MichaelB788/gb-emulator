#pragma once
#include "cartridge.h"
#include "constants.h"
#include "interrupts.h"
#include "joypad.h"
#include "serial.h"
#include "timer.h"

struct bus {
  struct cartridge *cart;
  struct joypad joypad;
  struct interrupts interrupts;
  struct serial serial;
  struct timer timer;

  uint8_t vram[KiB_8];
  uint8_t wram[KiB_8];
  uint8_t hram[127];
};

void bus_init(struct bus *bus, struct cartridge *cart);

void bus_tick(struct bus *bus); // Advances by 1 M-cycle / 4 T-cycles

// Read and writes

[[nodiscard]] uint8_t bus_read_byte(const struct bus *bus, uint16_t addr);

void bus_write_byte(struct bus *bus, uint16_t addr, uint8_t val);
