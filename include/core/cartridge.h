#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  ROM_ONLY_MAPPER,
  MBC1_MAPPER,
  MBC1_RAM_MAPPER,
  MBC1_RAM_BATTERY_MAPPER,
  MBC2_MAPPER,
  MBC2_BATTERY_MAPPER,
} CartridgeType;

typedef struct {
  bool advanced_banking_enabled;
  uint8_t primary_bank;
  uint8_t secondary_bank;
} MBC1;

typedef struct {
  CartridgeType type;
  union {
    MBC1 mbc1;
    // TODO: More mappers
  };
  bool ram_enabled;
  size_t rom_size;
  size_t ram_size;
  uint8_t *rom;
  uint8_t *ram;
} Cartridge;

// Returns a heap allocated cartridge on success, NULL on failure
Cartridge *create_cartridge(const char *path_to_rom);

// Destroys a cartridge if it exists, otherwise this operation is a no-op.
void destroy_cartridge(Cartridge *cartridge);

uint8_t read_rom(const Cartridge *cartridge, uint16_t addr);

uint8_t read_ram(Cartridge *cartridge, uint16_t addr);

void write_rom(Cartridge *cartridge, uint16_t addr, uint8_t val);

void write_ram(Cartridge *cartridge, uint16_t addr, uint8_t val);
