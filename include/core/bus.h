#pragma once
#include "core/cartridge.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  Cartridge *cartridge;
  bool IE;
} Bus;

uint8_t read_byte(Bus *bus, uint16_t addr);

void write_byte(Bus *bus, uint16_t addr, uint8_t val);
