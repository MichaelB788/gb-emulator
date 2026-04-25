#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  MAPPER_ROM_ONLY,
  MAPPER_MBC1,
  MAPPER_MBC1_RAM,
  MAPPER_MBC1_RAM_BATTERY,
  MAPPER_MBC2,
  MAPPER_MBC2_BATTERY,
} Mapper;

typedef struct {
  Mapper mapper;

  uint8_t *rom;
  uint8_t *ram;

  size_t rom_size;
  size_t ram_size;

  bool ram_enabled;
  uint8_t rom_bank;
  uint8_t ram_bank;
} Cartridge;

// Returns a heap allocated cartridge on success, NULL on failure
Cartridge *create_cartridge(const char *path_to_rom);

// Destroys a cartridge if it exists, otherwise this operation is a no-op.
void destroy_cartridge(Cartridge *cartridge);

uint8_t read_rom(Cartridge *cartridge, uint16_t addr);

uint8_t read_ram(Cartridge *cartridge, uint16_t addr);

void write_rom(Cartridge *cartridge, uint16_t addr, uint8_t val);

void write_ram(Cartridge *cartridge, uint16_t addr, uint8_t val);
