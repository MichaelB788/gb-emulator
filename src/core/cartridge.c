#include "core/cartridge.h"
#include "core/mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool is_invalid_rom_file(FILE *file) {
  if (!file) {
    fprintf(stderr, "ROM file is NULL\n");
    return true;
  }
  return false;
}

bool is_invalid_cartridge(Cartridge *cartridge) {
  if (!cartridge) {
    fprintf(stderr, "Cartridge is NULL\n");
    return true;
  }
  return false;
}

bool init_ram(Cartridge *cartridge) {
  if (is_invalid_cartridge(cartridge)) {
    return false;
  }
  if (cartridge->ram_size == 0) {
    return true; // No RAM, so skip initialization and assume success.
  }

  // Cartridge has RAM, so initialize it.
  cartridge->ram = malloc(cartridge->ram_size);
  if (!cartridge->ram) {
    perror("RAM malloc failed");
    return false;
  }
  return true;
}

bool init_rom(FILE *rom_file, Cartridge *cartridge) {
  if (is_invalid_rom_file(rom_file) || is_invalid_cartridge(cartridge)) {
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

  rewind(rom_file);
  if (fread(cartridge->rom, 1, cartridge->rom_size, rom_file) !=
      cartridge->rom_size) {
    perror("Could not read ROM file");
    return false;
  }

  return true;
}

bool read_header(FILE *rom_file, Cartridge *cartridge) {
  if (is_invalid_rom_file(rom_file) || is_invalid_cartridge(cartridge)) {
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
  uint8_t header[HEADER_SIZE] = {0};
  if (fread(header, 1, HEADER_SIZE, rom_file) != HEADER_SIZE) {
    perror("Could not read ROM header into memory");
    return false;
  }

  // Note: A RAM size byte with value 0x1 isn't used by any official ROMS, so
  // the actual size is unknown. I just assume no RAM.
  const size_t ram_sizes[6] = {0, 0, 8000, 32000, 128000, 64000};
  cartridge->type = header[MAPPER_BYTE];
  cartridge->rom_size = 32000 * (1 << header[ROM_SIZE_BYTE]);
  cartridge->ram_size = ram_sizes[header[RAM_SIZE_BYTE]];

  return true;
}

void init_mapper(Cartridge *cart) {
  switch (cart->type) {
  case MAPPER_MBC1:
    init_mbc1(&cart->mbc1);
    break;
  default:
    fprintf(stderr, "Cannot init mapper\n");
    break;
  }
}

Cartridge *create_cartridge(const char *path_to_rom) {
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not read ROM file");
    return NULL;
  }

  Cartridge *cartridge = malloc(sizeof(Cartridge));
  cartridge->rom = NULL;
  cartridge->ram = NULL;
  cartridge->ram_enabled = false;

  // It's important to call read_header() before initializing ROM or RAM as
  // we'll need to know the appropriate sizes for both before allocating memory.
  if (read_header(rom_file, cartridge) && init_rom(rom_file, cartridge) &&
      init_ram(cartridge)) {
    init_mapper(cartridge);
  } else {
    destroy_cartridge(cartridge);
    cartridge = NULL;
  }

  fclose(rom_file);
  return cartridge;
}

void destroy_cartridge(Cartridge *cartridge) {
  if (cartridge) {
    free(cartridge->rom);
    free(cartridge->ram);
    free(cartridge);
  }
}

uint8_t read_rom(const Cartridge *cartridge, const uint16_t addr) {
  switch (cartridge->type) {
  case MAPPER_ROM_ONLY:
    return cartridge->rom[addr];
  case MAPPER_MBC1:
  case MAPPER_MBC1_RAM:
  case MAPPER_MBC1_RAM_BATTERY:
    return read_mbc1_rom(cartridge, addr);
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

uint8_t read_ram(Cartridge *cartridge, const uint16_t addr) {
  switch (cartridge->type) {
  case MAPPER_ROM_ONLY:
  case MAPPER_MBC1:
  case MAPPER_MBC2:
    return 0xFF; // This is technically undefined behavior. The value returned
                 // can really be anything.
  case MAPPER_MBC1_RAM:
  case MAPPER_MBC1_RAM_BATTERY:
    return read_mbc1_ram(cartridge, addr);
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

void write_rom(Cartridge *cartridge, const uint16_t addr, const uint8_t val) {
  switch (cartridge->type) {
  case MAPPER_ROM_ONLY:
    break; // No MBC to handle writes, so this is a no op.
  case MAPPER_MBC1:
  case MAPPER_MBC1_RAM:
  case MAPPER_MBC1_RAM_BATTERY:
    write_mbc1_rom(cartridge, addr, val);
    break;
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    break;
  }
}

void write_ram(Cartridge *cartridge, const uint16_t addr, const uint8_t val) {
  switch (cartridge->type) {
  case MAPPER_ROM_ONLY:
  case MAPPER_MBC1:
  case MAPPER_MBC2:
    break; // No RAM available, so this is a no op.
  case MAPPER_MBC1_RAM:
  case MAPPER_MBC1_RAM_BATTERY:
    write_mbc1_ram(cartridge, addr, val);
    break;
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    break;
  }
}
