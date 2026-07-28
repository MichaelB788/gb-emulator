#pragma once
#include <stdbool.h>
#include <stdint.h>

struct cartridge;

enum mapper {
  ROM_ONLY_CART = 0x00,
  MBC1_CART = 0x01,
  MBC1_RAM_CART = 0x02,
  MBC1_RAM_BATTERY_CART = 0x03,
  MBC2_CART = 0x05,
  MBC2_BATTERY_CART = 0x06,
};

bool mapper_init(struct cartridge *cart, uint8_t mapper_header_byte);
bool mapper_has_ram(enum mapper mapper);

uint8_t mapper_read_rom(const struct cartridge *cart, uint16_t addr);
void mapper_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val);

uint8_t mapper_read_ram(const struct cartridge *cart, uint16_t addr);
void mapper_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val);
