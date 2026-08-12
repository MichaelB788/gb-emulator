#pragma once
#include "mbc1.h"
#include <stdbool.h>
#include <stdint.h>

struct cartridge;
struct u8_fixed_vec;

enum mapper_type {
  ROM_ONLY_CART = 0x00,
  MBC1_CART = 0x01,
  MBC1_RAM_CART = 0x02,
  MBC1_RAM_BATTERY_CART = 0x03,
  MBC2_CART = 0x05,
  MBC2_BATTERY_CART = 0x06,
  MAPPER_UNKNOWN
};

struct mapper {
  enum mapper_type type;
  union {
    struct mbc1 mbc1;
  };
};

/// `cart_type` is derived from reading 0x147 in the header
bool create_mapper(struct mapper *mapper, uint8_t mapper_type);

uint8_t mapper_read_rom(const struct mapper *mapper,
                        const struct u8_fixed_vec *rom, uint16_t addr);
void mapper_write_rom(struct mapper *mapper, const struct u8_fixed_vec *rom,
                      uint16_t addr, uint8_t val);

uint8_t mapper_read_ram(const struct mapper *mapper,
                        const struct u8_fixed_vec *ram, uint16_t addr);
void mapper_write_ram(const struct mapper *mapper, struct u8_fixed_vec *ram,
                      uint16_t addr, uint8_t val);
