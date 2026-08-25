#pragma once
#include "mbc1.h"
#include "u8_buf.h"
#include <stddef.h>
#include <stdint.h>

enum cartridge_type {
  ROM_ONLY_CART = 0x00,
  MBC1_CART = 0x01,
  MBC1_RAM_CART = 0x02,
  MBC1_RAM_BATTERY_CART = 0x03,
};

struct cartridge {
  enum cartridge_type type;
  union {
    struct mbc1 mbc1;
  };
  struct u8_buf rom;
  struct u8_buf ram;
};

// Create / Destroy

[[nodiscard]] bool cartridge_create(struct cartridge *cart,
                                    const char *rom_path);

void cartridge_destroy(struct cartridge *cart);

// Read and writes

[[nodiscard]] uint8_t cartridge_read_rom(const struct cartridge *cart,
                                         uint16_t addr);

[[nodiscard]] uint8_t cartridge_read_ram(const struct cartridge *cart,
                                         uint16_t addr);

void cartridge_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val);

void cartridge_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val);
