#pragma once
#include "core/cartridge.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  bool interrupt_enable;
  uint8_t wram[0x2000];
  uint8_t hram[127];
  Cartridge *cartridge;
} Bus;

uint8_t read_byte(Bus *bus, uint16_t addr);

void write_byte(Bus *bus, uint16_t addr, uint8_t val);
