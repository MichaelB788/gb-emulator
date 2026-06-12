#pragma once
#include "mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  ROM_MAPPER,
  MBC1_MAPPER,
} Mapper;

typedef struct Cartridge {
  Mapper mapper;
  union {
    MBC1 mbc1;
  };

  uint8_t *rom;
  size_t rom_size;
  uint8_t *ram;
  size_t ram_size;
} Cartridge;

// Returns a heap allocated cartridge on success, NULL on failure
Cartridge *create_cartridge(const char *path_to_rom);

// Destroys a cartridge if it exists, otherwise this operation is a no-op.
void destroy_cartridge(Cartridge *cart);

uint8_t read_rom(const Cartridge *cart, uint16_t addr);

void write_rom(Cartridge *cart, uint16_t addr, uint8_t val);

uint8_t read_ram(Cartridge *cart, uint16_t addr);

void write_ram(Cartridge *cart, uint16_t addr, uint8_t val);
