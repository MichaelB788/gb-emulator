#include "core/cartridge.h"
#include "core/mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_mapper(Cartridge *cart) {
  switch (cart->type) {
  case MAPPER_MBC1:
    init_mbc1(&cart->mbc1);
    break;
  default:
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
      perror("Could not read ROM header into memory");
      goto cleanup;
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
    goto cleanup;
  }
  if (fread(cartridge->rom, 1, cartridge->rom_size, rom_file) !=
      cartridge->rom_size) {
    perror("Could not read ROM file");
    goto cleanup;
  }

  // Initialize RAM if the cartridge type has it.
  if (cartridge->ram_size > 0) {
    cartridge->ram = malloc(cartridge->ram_size);
    if (!cartridge->ram) {
      perror("RAM malloc failed");
      goto cleanup;
    }
  }

  fclose(rom_file);
  return cartridge;

cleanup:
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
