#pragma once
#include "mapper.h"
#include "mbc1.h"
#include "vector.h"
#include <stdbool.h>
#include <stdint.h>

struct cartridge {
  struct mapper mapper;
  struct u8_fixed_vec rom;
  struct u8_fixed_vec ram;
};

bool create_cartridge(struct cartridge *cart, const char *path_to_rom);
void destroy_cartridge(struct cartridge *cart);

uint8_t cartridge_read_rom(const struct cartridge *cart, uint16_t addr);
void cartridge_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val);

uint8_t cartridge_read_ram(const struct cartridge *cart, uint16_t addr);
void cartridge_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val);
