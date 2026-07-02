#pragma once
#include "mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum cartridge_type {
  ROM_ONLY_CART = 0x00,
  MBC1_CART = 0x01,
  MBC1_RAM_CART = 0x02,
  MBC1_RAM_BATTERY_CART = 0x03,
  MBC2_CART = 0x05,
  MBC2_BATTERY_CART = 0x06,
};

struct cartridge {
  enum cartridge_type type;
  union {
    struct mbc1 mbc1;
  };

  size_t rom_size;
  size_t ram_size;

  uint8_t *rom;
  uint8_t *ram;
};

bool init_cartridge(struct cartridge *cart, const char *path_to_rom);

void destroy_cartridge(struct cartridge *cart);

uint8_t rom_read(const struct cartridge *cart, uint16_t addr);

void rom_write(struct cartridge *cart, uint16_t addr, uint8_t val);

uint8_t ram_read(struct cartridge *cart, uint16_t addr);

void ram_write(struct cartridge *cart, uint16_t addr, uint8_t val);
