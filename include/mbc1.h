#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * Collection of functions for reading and writing to MBC1.
 */

typedef struct {
  uint8_t rom_bank;
  uint8_t ram_bank;

  bool advanced_banking_enabled;
  bool ram_enabled;

  size_t rom_size;
  size_t ram_size;

  uint8_t *rom;
  uint8_t *ram;
} MBC1;

void init_mbc1(MBC1 *mbc1);

uint8_t read_mbc1_rom(const MBC1 *mbc1, uint16_t addr);

void write_mbc1_rom(MBC1 *mbc1, uint16_t addr, uint8_t val);

uint8_t read_mbc1_ram(const MBC1 *mbc1, uint16_t addr);

void write_mbc1_ram(MBC1 *mbc1, uint16_t addr, uint8_t val);
