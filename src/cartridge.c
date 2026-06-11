#include "cartridge.h"
#include "mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_mapper(Cartridge *cart) {
  switch (cart->type) {
  case MBC1_MAPPER:
    init_mbc1(&cart->mbc1);
    break;
  default:
    fprintf(stderr, "Error: Cannot init unknown mapper\n");
    break;
  }
}

Cartridge *create_cartridge(const char *path_to_rom) {
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not open file from path");
    return NULL;
  }

  Cartridge *cartridge = malloc(sizeof(Cartridge));
  cartridge->rom = NULL;
  cartridge->ram = NULL;
  cartridge->ram_enabled = false;

  // Read the cartridge header into memory
  {
    // Important byte addresses + header size. See:
    // https://gbdev.io/pandocs/The_Cartridge_Header.html
    enum {
      MAPPER_BYTE = 0x147,
      ROM_SIZE_BYTE = 0x148,
      RAM_SIZE_BYTE = 0x149,
      HEADER_END = 0x150
    };
    uint8_t header[HEADER_END] = {0};
    if (fread(header, 1, HEADER_END, rom_file) != HEADER_END) {
      perror("Could not read header from file");
      goto error;
    }
    rewind(rom_file);

    // Note: A RAM size byte with value 0x1 isn't used by any official ROMS, so
    // the actual size is unknown. I just assume no RAM exists.
    const size_t ram_sizes[6] = {0, 0, 8000, 32000, 128000, 64000};
    cartridge->type = header[MAPPER_BYTE];
    cartridge->rom_size = 32000 * (1 << header[ROM_SIZE_BYTE]);
    cartridge->ram_size = ram_sizes[header[RAM_SIZE_BYTE]];
    init_mapper(cartridge);
  }

  // Read the ROM file into memory
  cartridge->rom = malloc(cartridge->rom_size);
  if (!cartridge->rom) {
    perror("ROM malloc failed");
    goto error;
  }
  if (fread(cartridge->rom, 1, cartridge->rom_size, rom_file) !=
      cartridge->rom_size) {
    perror("Could not read ROM from file");
    goto error;
  }

  // Initialize RAM if the cartridge type has it.
  if (cartridge->ram_size > 0) {
    cartridge->ram = malloc(cartridge->ram_size);
    if (!cartridge->ram) {
      perror("RAM malloc failed");
      goto error;
    }
  }

  fclose(rom_file);
  return cartridge;

error:
  fclose(rom_file);
  destroy_cartridge(cartridge);
  return NULL;
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
  // ROM Only
  case ROM_ONLY_MAPPER: {
    return cartridge->rom[addr];
  }

  // MBC1 mappers
  case MBC1_MAPPER:
  case MBC1_RAM_MAPPER:
  case MBC1_RAM_BATTERY_MAPPER: {
    return read_mbc1_rom(cartridge, addr);
  }

  // Error
  default:
    fprintf(stderr, "Error: ROM read to unimplemented mapper\n");
    return 0x10;
  }
}

void write_rom(Cartridge *cartridge, const uint16_t addr, const uint8_t val) {
  switch (cartridge->type) {
  // ROM is read only, so this is a no-op.
  case ROM_ONLY_MAPPER:
    break;

  // MBC1 mappers
  case MBC1_MAPPER:
  case MBC1_RAM_MAPPER:
  case MBC1_RAM_BATTERY_MAPPER:
    write_mbc1_rom(cartridge, addr, val);
    break;

  // Error
  default:
    fprintf(stderr, "Error: ROM write to unimplemented mapper\n");
    break;
  }
}

uint8_t read_ram(Cartridge *cartridge, const uint16_t addr) {
  switch (cartridge->type) {
  // Mappers have no RAM to read from
  case ROM_ONLY_MAPPER:
  case MBC1_MAPPER:
  case MBC2_MAPPER: {
    return 0xFF;
  }

  // MBC1 mappers with built in RAM
  case MBC1_RAM_MAPPER:
  case MBC1_RAM_BATTERY_MAPPER: {
    return read_mbc1_ram(cartridge, addr);
  }

  // Error
  default:
    fprintf(stderr, "Error: RAM read to unimplemented mapper\n");
    return 0x10;
  }
}

void write_ram(Cartridge *cartridge, const uint16_t addr, const uint8_t val) {
  switch (cartridge->type) {
  // Mappers with no ram.
  case ROM_ONLY_MAPPER:
  case MBC1_MAPPER:
  case MBC2_MAPPER:
    break;

  // MBC1 mappers with RAM
  case MBC1_RAM_MAPPER:
  case MBC1_RAM_BATTERY_MAPPER:
    write_mbc1_ram(cartridge, addr, val);
    break;

  // Error
  default:
    fprintf(stderr, "Error: RAM write to unimplemented mapper\n");
    break;
  }
}
