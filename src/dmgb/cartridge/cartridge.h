#pragma once
#include "mbc1.h"
#include "u8_array.h"
#include <stdbool.h>
#include <stdint.h>

#define ROM_ONLY_CART 0x00u
#define MBC1_CART 0x01u
#define MBC1_RAM_CART 0x02u
#define MBC1_RAM_BATTERY_CART 0x03u

// todo
union cartridge_mapper {
  struct mbc1 mbc1;
};

struct cartridge {
  uint8_t type;
  union cartridge_mapper mapper;

  struct u8_array rom;
  struct u8_array ram;
};

bool cartridge_create(struct cartridge *cart, const char *rom_path);
void cartridge_destroy(struct cartridge *cart);

uint8_t cartridge_read_rom(const struct cartridge *cart, uint16_t addr);
void cartridge_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val);

void cartridge_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val);
uint8_t cartridge_read_ram(const struct cartridge *cart, uint16_t addr);
