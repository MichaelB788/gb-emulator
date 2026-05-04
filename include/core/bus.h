#pragma once
#include "core/cartridge.h"
#include <stdbool.h>
#include <stdint.h>

typedef struct {
  Cartridge *cartridge;
  uint8_t wram[8192]; // 8 KiB of work ram. Does not assume bank switching, as
                      // it's a feature of the GameBoy Color.
  uint8_t hram[127];
  bool interrupt_enable;
} Bus;

uint8_t read_byte(Bus *bus, uint16_t addr);

void write_byte(Bus *bus, uint16_t addr, uint8_t val);
