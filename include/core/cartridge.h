#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef enum {
  ROM_ONLY,
  MBC1,
  MBC1_RAM,
  MBC1_RAM_BATTERY,
  MBC2,
  MBC2_BATTERY,
} CartridgeType;

typedef struct {
  CartridgeType type;

  uint8_t *rom;
  uint8_t *ram;

  size_t rom_size;
  size_t ram_size;

  uint8_t rom_bank;
  uint8_t ram_bank;
} Cartridge;

bool init_cartridge(Cartridge *cartridge, const char *path_to_rom);

void close_cartridge(Cartridge *cartridge);

uint8_t read_rom(Cartridge *cartridge, uint16_t addr);

uint8_t read_ram(Cartridge *cartridge, uint16_t addr);

void write_rom(Cartridge *cartridge, uint16_t addr, uint8_t val);

void write_ram(Cartridge *cartridge, uint16_t addr, uint8_t val);
