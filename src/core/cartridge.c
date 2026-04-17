#include "core/cartridge.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool initialize_ram(Cartridge *cartridge) {
  cartridge->ram = malloc(cartridge->ram_size);
  if (!cartridge->ram && cartridge->ram_size > 0) {
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

  const size_t bytes_read =
      fread(cartridge->rom, 1, cartridge->rom_size, rom_file);
  if (bytes_read != cartridge->rom_size) {
    perror("Could not read file contents into memory");
    return false;
  }

  return true;
}

bool read_header(FILE *rom_file, Cartridge *cartridge) {
  enum { HEADER_SIZE = 0x150 };

  uint8_t header[HEADER_SIZE] = {0};
  const size_t bytes_read = fread(header, 1, HEADER_SIZE, rom_file);
  if (bytes_read != HEADER_SIZE) {
    perror("Could not read ROM header into memory");
    return false;
  }

  cartridge->type = header[0x147];
  cartridge->rom_size = 32000 * (1 << header[0x148]);
  const size_t ram_sizes[6] = {0, 0, 8000, 32000, 128000, 64000};
  cartridge->ram_size = ram_sizes[cartridge->rom[0x149]];

  return true;
}

bool init_cartridge(Cartridge *cartridge, const char *path_to_rom) {
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not read ROM file");
    return false;
  }

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
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

uint8_t read_ram(Cartridge *cartridge, uint16_t addr) {
  switch (cartridge->type) {
  case ROM_ONLY:
    return 0x00;
  default:
    fprintf(stderr, "Attempting read from unimplemented cartridge type\n");
    return 0x10;
  }
}

void write_rom(Cartridge *cartridge, uint16_t addr, uint8_t val) {
  switch (cartridge->type) {
  case ROM_ONLY:
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    (void)val;
    (void)addr;
    break;
  }
}

void write_ram(Cartridge *cartridge, uint16_t addr, uint8_t val) {
  switch (cartridge->type) {
  case ROM_ONLY:
    (void)val;
    (void)addr;
    break;
  default:
    fprintf(stderr, "Attempting write from unimplemented cartridge type\n");
    (void)val;
    (void)addr;
  }
}
