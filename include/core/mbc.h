#pragma once
#include "core/cartridge.h"
#include <stdint.h>

// Collection of functions for reading and writing to MBC's.

// MBC1 read/writes
uint8_t read_mbc1_rom(const Cartridge *cartridge, uint16_t addr);

void write_mbc1_rom(Cartridge *cartridge, uint16_t addr, uint8_t val);

uint8_t read_mbc1_ram(Cartridge *cartridge, uint16_t addr);

void write_mbc1_ram(Cartridge *cartridge, uint16_t addr, uint8_t val);
