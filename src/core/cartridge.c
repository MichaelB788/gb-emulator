#include "core/cartridge.h"
#include "core/mbc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool initialize_ram(Cartridge *cartridge) {
  cartridge->ram = malloc(cartridge->ram_size);
  if (cartridge->ram_size > 0 && !cartridge->ram) {
    perror("Error allocating memory for RAM");
    return false;
  }

  return true;
}

bool read_rom_into_memory(FILE *rom_file, Cartridge *cartridge) {
  cartridge->rom = malloc(cartridge->rom_size);
  if (!cartridge->rom) {
    perror("Error allocating memory for ROM");
    return false;
  }

  if (fread(cartridge->rom, 1, cartridge->rom_size, rom_file) !=
      cartridge->rom_size) {
    perror("Could not read file contents into memory");
    return false;
  }

  return true;
}

bool read_header(FILE *rom_file, Cartridge *cartridge) {
  enum { HEADER_SIZE = 0x150 };

  // Load the cartridge header into a temporary buffer
  uint8_t header[HEADER_SIZE] = {0};
  if (fread(header, 1, HEADER_SIZE, rom_file) != HEADER_SIZE) {
    perror("Could not read ROM header into memory");
    return false;
  }

  // Refer to: https://gbdev.io/pandocs/The_Cartridge_Header.html
  const size_t ram_sizes[6] = {0, 0, 8000, 32000, 128000, 64000};
  cartridge->type = header[0x147];
  cartridge->rom_size = 32000 * (1 << header[0x148]);
  cartridge->ram_size = ram_sizes[header[0x149]];

  return true;
}

bool init_cartridge(Cartridge *cartridge, const char *path_to_rom) {
  // Initialize variables
  cartridge->rom = NULL;
  cartridge->ram = NULL;
  cartridge->rom_bank = 1;
  cartridge->ram_enabled = false;

  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not read ROM file");
    return false;
  }

  // Expression chain will short circuit if any errors occur. It is important
  // that these functions are called in this order.
  bool success = read_header(rom_file, cartridge) &&
                 read_rom_into_memory(rom_file, cartridge) &&
                 initialize_ram(cartridge);

  fclose(rom_file);
  return success;
}

void close_cartridge(Cartridge *cartridge) {
  if (cartridge->rom) {
    free(cartridge->rom);
    cartridge->rom = NULL;
  }

  if (cartridge->ram) {
    free(cartridge->ram);
    cartridge->ram = NULL;
  }
}

uint8_t read_rom(Cartridge *cartridge, uint16_t addr) {
  switch (cartridge->type) {
  case ROM_ONLY:
    return cartridge->rom[addr];
  case MBC1:
    return read_mbc1_rom(cartridge, addr);
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

uint8_t read_ram(Cartridge *cartridge, uint16_t addr) {
  switch (cartridge->type) {
  // These MBC's have no RAM
  case ROM_ONLY:
  case MBC1:
    return 0x00;
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

void write_rom(Cartridge *cartridge, uint16_t addr, uint8_t val) {
  switch (cartridge->type) {
  // These MBC's have no RAM, so this action is a NOP.
  case ROM_ONLY:
  case MBC1:
    (void)addr;
    (void)val;
    break;
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    break;
  }
}

void write_ram(Cartridge *cartridge, uint16_t addr, uint8_t val) {
  switch (cartridge->type) {
  // These MBC's have no RAM, so this action is a NOP.
  case ROM_ONLY:
  case MBC1:
    (void)val;
    (void)addr;
    break;
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    break;
  }
}
