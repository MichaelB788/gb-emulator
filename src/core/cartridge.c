#include "core/cartridge.h"
#include "core/mbc.h"
#include <complex.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool invalid_rom_file(FILE *file) {
  if (!file) {
    fprintf(stderr, "ROM file is NULL\n");
    return true;
  }

  return false;
}

bool invalid_cartridge(Cartridge *cartridge) {
  if (!cartridge) {
    fprintf(stderr, "Cartridge is NULL\n");
    return true;
  }

  return false;
}

bool initialize_ram(Cartridge *cartridge) {
  if (invalid_cartridge(cartridge)) {
    return false;
  }
  if (cartridge->ram_size == 0) {
    // No RAM, so skip initialization and assume success.
    return true;
  }

  // Cartridge has RAM, so initialize it.
  cartridge->ram = malloc(cartridge->ram_size);
  if (!cartridge->ram) {
    perror("RAM malloc failed");
    return false;
  }

  return true;
}

bool initialize_rom(FILE *rom_file, Cartridge *cartridge) {
  if (invalid_rom_file(rom_file) || invalid_cartridge(cartridge)) {
    return false;
  }
  if (cartridge->rom_size < 32000) {
    fprintf(stderr, "Invalid ROM size: %zu\n", cartridge->rom_size);
    return false;
  }

  // Attempt to read ROM file bytes into memory.
  cartridge->rom = malloc(cartridge->rom_size);
  if (!cartridge->rom) {
    perror("ROM malloc failed");
    return false;
  }
  if (fread(cartridge->rom, 1, cartridge->rom_size, rom_file) !=
      cartridge->rom_size) {
    perror("Could not ROM file");
    return false;
  }

  return true;
}

bool read_header(FILE *rom_file, Cartridge *cartridge) {
  if (invalid_rom_file(rom_file) || invalid_cartridge(cartridge)) {
    return false;
  }

  // Important byte addresses + header size. See:
  // https://gbdev.io/pandocs/The_Cartridge_Header.html
  enum {
    MAPPER_BYTE = 0x147,
    ROM_SIZE_BYTE = 0x148,
    RAM_SIZE_BYTE = 0x149,
    HEADER_SIZE = 0x150
  };

  // Read the first 0x150 bytes into a temporary buffer.
  uint8_t header[HEADER_SIZE] = {0};
  if (fread(header, 1, HEADER_SIZE, rom_file) != HEADER_SIZE) {
    perror("Could not read ROM header into memory");
    return false;
  }

  // Note: A RAM size byte with value 0x1 isn't used by any official ROMS, so
  // the actual size is unknown. I just assume no RAM.
  const size_t ram_sizes[6] = {0, 0, 8000, 32000, 128000, 64000};

  // Parse the header.
  cartridge->mapper = header[MAPPER_BYTE];
  cartridge->rom_size = 32000 * (1 << header[ROM_SIZE_BYTE]);
  cartridge->ram_size = ram_sizes[header[RAM_SIZE_BYTE]];

  // See: https://gbdev.io/pandocs/MBC1.html#registers
  cartridge->rom_bank = 0x01;

  return true;
}

Cartridge *create_cartridge(const char *path_to_rom) {
  // Read the file in binary mode.
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not read ROM file");
    return NULL;
  }

  // It's important to call read_header() before initializing ROM or RAM as
  // we'll need to know the appropriate sizes for both before allocating memory.
  Cartridge *cartridge = (Cartridge *)malloc(sizeof(Cartridge));
  bool success = read_header(rom_file, cartridge) &&
                 initialize_rom(rom_file, cartridge) &&
                 initialize_ram(cartridge);

  if (!success) {
    destroy_cartridge(cartridge);
  }
  fclose(rom_file);
  return cartridge;
}

void destroy_cartridge(Cartridge *cartridge) {
  if (cartridge) {
    if (cartridge->rom) {
      free(cartridge->rom);
      cartridge->rom = NULL;
    }

    if (cartridge->ram) {
      free(cartridge->ram);
      cartridge->ram = NULL;
    }

    free(cartridge);
    cartridge = NULL;
  }
}

uint8_t read_rom(Cartridge *cartridge, uint16_t addr) {
  switch (cartridge->mapper) {
  case MAPPER_ROM_ONLY:
    return cartridge->rom[addr];
  case MAPPER_MBC1:
    return read_mbc1_rom(cartridge, addr);
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

uint8_t read_ram(Cartridge *cartridge, uint16_t addr) {
  switch (cartridge->mapper) {
  // These MBC's have no RAM
  case MAPPER_ROM_ONLY:
  case MAPPER_MBC1:
    return 0x00;
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

void write_rom(Cartridge *cartridge, uint16_t addr, uint8_t val) {
  switch (cartridge->mapper) {
  // These MBC's have no RAM, so this action is a NOP.
  case MAPPER_ROM_ONLY:
  case MAPPER_MBC1:
    (void)addr;
    (void)val;
    break;
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    break;
  }
}

void write_ram(Cartridge *cartridge, uint16_t addr, uint8_t val) {
  switch (cartridge->mapper) {
  // These MBC's have no RAM, so this action is a NOP.
  case MAPPER_ROM_ONLY:
  case MAPPER_MBC1:
    (void)val;
    (void)addr;
    break;
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    break;
  }
}
