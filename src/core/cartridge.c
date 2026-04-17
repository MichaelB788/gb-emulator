#include "core/cartridge.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum { HEADER_SIZE = 0x150 };

bool init_cartridge(Cartridge *cartridge, const char *path_to_rom) {
  bool success = true;
  FILE *rom_file;

  uint8_t header[HEADER_SIZE] = {0};
  rom_file = fopen(path_to_rom, "rb");
  if (rom_file) {
    const size_t header_bytes_read =
        fread(header, sizeof header[0], HEADER_SIZE, rom_file);
    if (header_bytes_read == HEADER_SIZE) {
      cartridge->type = header[0x147];

      const uint8_t rom_size_byte = header[0x148];
      cartridge->rom_size =
          rom_size_byte != 0 ? 32000 * (1 << rom_size_byte) : 0;

      cartridge->rom = malloc(cartridge->rom_size);
      const size_t bytes_read =
          fread(cartridge->rom, 1, cartridge->rom_size, rom_file);
      if (bytes_read == cartridge->rom_size) {
        const size_t ram_sizes[6] = {0, 0, 8000, 32000, 128000, 64000};
        cartridge->ram_size = ram_sizes[cartridge->rom[0x149]];
        cartridge->ram = malloc(cartridge->ram_size);
      } else {
        success = false;
        perror("Could not read ROM");
      }
    } else {
      success = false;
      perror("Failed to read bytes into header");
    }
  } else {
    success = false;
    perror("Could not read ROM file");
  }

  fclose(rom_file);
  return true;
}

void close_cartridge(Cartridge *cartridge) {
  free(cartridge->rom);
  free(cartridge->ram);
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
