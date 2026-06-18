#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct mbc1 {
  uint8_t rom_bank;
  uint8_t ram_bank;
  bool advanced_banking_enabled;
  bool ram_enabled;
};

void init_mbc1(struct mbc1 *mbc1);

struct cartridge;

uint8_t mbc1_read_rom(const struct cartridge *cart, uint16_t addr);

void mbc1_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val);

uint8_t mbc1_read_ram(const struct cartridge *cart, uint16_t addr);

void mbc1_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val);
