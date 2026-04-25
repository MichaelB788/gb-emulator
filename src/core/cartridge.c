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

  enum { HEADER_SIZE = 0x150 };

  // Read the first 0x150 bytes into a temporary buffer.
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

Cartridge *create_cartridge(const char *path_to_rom) {
  static Cartridge cartridge = {0};

  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not read ROM file");
    return NULL;
  }

  // It's important to read the header before initializing ROM and RAM, as it
  // contains important information about MBC type and ROM/RAM size, which will
  // then be used in initalize_rom and initialize_ram.
  bool success = true;
  if (read_header(rom_file, &cartridge)) {
    success =
        initialize_rom(rom_file, &cartridge) && initialize_ram(&cartridge);
  } else {
    success = false;
  }

  fclose(rom_file);

  return success ? &cartridge : NULL;
}

void close_cartridge(Cartridge *cartridge) {
  if (cartridge) {
    if (cartridge->rom) {
      free(cartridge->rom);
      cartridge->rom = NULL;
    }

    if (cartridge->ram) {
      free(cartridge->ram);
      cartridge->ram = NULL;
    }
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
