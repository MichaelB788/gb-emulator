#pragma once
#include "mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

enum mapper {
  ROM_MAPPER,
  MBC1_MAPPER,
};

struct cartridge {
  enum mapper mapper;
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

uint8_t read_rom(const struct cartridge *cart, uint16_t addr);

void write_rom(struct cartridge *cart, uint16_t addr, uint8_t val);

uint8_t read_ram(struct cartridge *cart, uint16_t addr);

void write_ram(struct cartridge *cart, uint16_t addr, uint8_t val);
