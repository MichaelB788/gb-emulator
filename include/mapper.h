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
};

struct mapper {
  enum mapper_type type;
  struct mbc1 mbc1;
};

struct mapper mapper_create(enum mapper_type type);

uint8_t mapper_read_rom(struct mapper mapper, const struct u8_fixed_vec *rom,
                        uint16_t addr);

void mapper_write_rom(struct mapper *mapper, const struct u8_fixed_vec *rom,
                      uint16_t addr, uint8_t val);

uint8_t mapper_read_ram(struct mapper mapper, const struct u8_fixed_vec *ram,
                        uint16_t addr);

void mapper_write_ram(struct mapper mapper, struct u8_fixed_vec *ram,
                      uint16_t addr, uint8_t val);
