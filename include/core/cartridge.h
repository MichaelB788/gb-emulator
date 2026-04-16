#pragma once
#include "core/mbc.h"
#include <stdint.h>

typedef struct {
  MBC mbc;
} Cartridge;

void init_cartridge(Cartridge *cart, const char *path_to_rom);

uint8_t cartridge_read(Cartridge *cart, uint16_t addr);

void cartridge_write(Cartridge *cart, uint16_t addr, uint8_t val);
