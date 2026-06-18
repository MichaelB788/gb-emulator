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

  uint8_t *rom;
  size_t rom_size;
  uint8_t *ram;
  size_t ram_size;
};

// Returns a heap allocated cartridge on success, NULL on failure
struct cartridge *create_cartridge(const char *path_to_rom);

// Destroys a cartridge if it exists, otherwise this operation is a no-op.
void destroy_cartridge(struct cartridge *cart);

uint8_t read_rom(const struct cartridge *cart, uint16_t addr);

void write_rom(struct cartridge *cart, uint16_t addr, uint8_t val);

uint8_t read_ram(struct cartridge *cart, uint16_t addr);

void write_ram(struct cartridge *cart, uint16_t addr, uint8_t val);
