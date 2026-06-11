#pragma once
#include "cartridge.h"
#include "io.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  Cartridge *cartridge;
  uint8_t wram[8192];
  IO io;
  uint8_t hram[127];
  bool interrupt_enable;
} Bus;

uint8_t read_byte(Bus *bus, uint16_t addr);

void write_byte(Bus *bus, uint16_t addr, uint8_t val);
