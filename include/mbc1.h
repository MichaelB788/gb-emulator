#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
  uint8_t rom_bank;
  uint8_t ram_bank;
  bool advanced_banking_enabled;
  bool ram_enabled;
} MBC1;

void init_mbc1(MBC1 *mbc1);

typedef struct Cartridge Cartridge;

uint8_t mbc1_read_rom(const Cartridge *cart, uint16_t addr);

void mbc1_write_rom(Cartridge *cart, uint16_t addr, uint8_t val);

uint8_t mbc1_read_ram(const Cartridge *cart, uint16_t addr);

void mbc1_write_ram(Cartridge *cart, uint16_t addr, uint8_t val);
