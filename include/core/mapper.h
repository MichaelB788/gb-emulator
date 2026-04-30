#pragma once
#include <stdint.h>

static inline uint16_t internal_rom_addr(uint8_t rom_bank, uint16_t addr) {
  return addr + (rom_bank * 0x4000);
}

uint16_t internal_ram_addr(uint8_t ram_bank, uint16_t addr);
