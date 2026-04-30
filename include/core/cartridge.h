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
} CartrideType;

typedef struct {
  bool advanced_banking_enabled;
  uint8_t primary_bank;
  uint8_t secondary_bank;
} MBC1;

typedef union {
  MBC1 mbc1;
  // TODO: More mappers
} Mapper;

typedef struct {
  CartrideType type;

  bool ram_enabled;

  Mapper mapper;

  size_t rom_size;
  size_t ram_size;

  uint8_t *rom;
  uint8_t *ram;
} Cartridge;

// Returns a heap allocated cartridge on success, NULL on failure
Cartridge *create_cartridge(const char *path_to_rom);

// Destroys a cartridge if it exists, otherwise this operation is a no-op.
void destroy_cartridge(Cartridge *cartridge);

uint8_t read_rom(Cartridge *cartridge, uint16_t addr);

uint8_t read_ram(Cartridge *cartridge, uint16_t addr);

void write_rom(Cartridge *cartridge, uint16_t addr, uint8_t val);

void write_ram(Cartridge *cartridge, uint16_t addr, uint8_t val);
