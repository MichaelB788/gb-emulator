#pragma once
#include "constants.h"
#include "interrupts.h"
#include "joypad.h"
#include "serial.h"
#include "timer.h"

struct cartridge;

struct bus {
  struct cartridge *cartridge;
  struct joypad joypad;
  struct interrupts interrupts;
  struct serial serial;
  struct timer timer;

  uint8_t vram[KiB_8];
  uint8_t wram[KiB_8];
  uint8_t hram[127];
};

bool bus_init(struct bus *bus, struct cartridge *cart);

// Ticks all subsytems by 1 M-cycle / 4 T-cycles
void bus_tick(struct bus *bus);

uint8_t bus_read_byte(const struct bus *bus, uint16_t addr);
void bus_write_byte(struct bus *bus, uint16_t addr, uint8_t val);

uint8_t bus_read_io(const struct bus *bus, uint16_t addr);
void bus_write_io(struct bus *bus, uint16_t addr, uint8_t val);
