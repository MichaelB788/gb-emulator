#pragma once
#include "byte_sizes.h"
#include "cartridge.h"
#include "interrupts.h"
#include "serial_transfer.h"
#include "timer.h"

struct bus {
  uint8_t joypad;

  struct interrupts interrupt;
  struct serial_transfer serial;
  struct timer timer;
  struct cartridge cartridge;

  uint8_t vram[KiB_8];
  uint8_t wram[KiB_8];
  uint8_t hram[127];
};

bool bus_init(struct bus *bus, const char *rom_path);
void bus_close(struct bus *bus);

uint8_t bus_read_byte(struct bus *bus, uint16_t addr);
void bus_write_byte(struct bus *bus, uint16_t addr, uint8_t val);

uint8_t bus_read_io(struct bus *bus, uint16_t addr);
void bus_write_io(struct bus *bus, uint16_t addr, uint8_t val);
