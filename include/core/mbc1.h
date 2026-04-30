#pragma once
#include "core/cartridge.h"
#include <stdint.h>

/**
 * Collection of functions for reading and writing to MBC1.
 */

void init_mbc1(MBC1 *mbc1);

uint8_t read_mbc1_rom(const Cartridge *cartridge, uint16_t addr);

void write_mbc1_rom(Cartridge *cartridge, uint16_t addr, uint8_t val);

uint8_t read_mbc1_ram(Cartridge *cartridge, uint16_t addr);

void write_mbc1_ram(Cartridge *cartridge, uint16_t addr, uint8_t val);
