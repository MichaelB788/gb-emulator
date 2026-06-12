#include "cartridge.h"
#include "byte_sizes.h"
#include "mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_mapper(Cartridge *cart) {
  switch (cart->mapper) {
  case ROM_MAPPER:
    break;

  case MBC1_MAPPER:
    init_mbc1(&cart->mbc1);
    break;
  }
}

bool read_header(FILE *rom_file, Cartridge *cart) {
  uint8_t header[0x150] = {0};
  if (fread(header, 1, 0x150, rom_file) != 0x150) {
    perror("Could not read file content into header");
    return false;
  }

  /**
   * Note: A RAM size byte with value 0x1 isn't used by any official ROMS, so
   * the actual size is unknown. I just assume no RAM exists.
   */
  const size_t ram_sizes[6] = {0, 0, KiB_8, KiB_32, KiB_128, KiB_64};
  cart->mapper = header[0x147];
  cart->rom_size = KiB_32 * (1 << header[0x148]);
  cart->ram_size = ram_sizes[header[0x149]];
  init_mapper(cart);

  return true;
}

bool initialize_rom(FILE *rom_file, Cartridge *cart) {
  rewind(rom_file);
  cart->rom = malloc(cart->rom_size);
  if (!cart->rom) {
    perror("Failed to malloc ROM");
    return false;
  }
  if (fread(cart->rom, 1, cart->rom_size, rom_file) != cart->rom_size) {
    perror("Could not read file content into ROM");
    return false;
  }
  return true;
}

bool initialize_ram(Cartridge *cart) {
  if (cart->ram_size > 0) {
    cart->ram = malloc(cart->ram_size);
    if (!cart->ram) {
      perror("RAM malloc failed");
      return false;
    }
  }
  return true;
}

Cartridge *create_cartridge(const char *path_to_rom) {
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not open file");
    return NULL;
  }

  Cartridge *cart = malloc(sizeof(Cartridge));
  if (read_header(rom_file, cart) && initialize_rom(rom_file, cart) &&
      initialize_ram(cart)) {
    fclose(rom_file);
    return cart;
  } else {
    fclose(rom_file);
    destroy_cartridge(cart);
    return NULL;
  }
}

void destroy_cartridge(Cartridge *cart) {
  if (cart) {
    free(cart->rom);
    free(cart->ram);
    free(cart);
  }
}

uint8_t read_rom(const Cartridge *cart, const uint16_t addr) {
  switch (cart->mapper) {
  case ROM_MAPPER:
    return cart->rom[addr];

  case MBC1_MAPPER:
    return mbc1_read_rom(cart, addr);
  }
}

void write_rom(Cartridge *cart, const uint16_t addr, const uint8_t val) {
  switch (cart->mapper) {
  case ROM_MAPPER:
    // ROM is read only, so this is a no-op.
    break;

  case MBC1_MAPPER:
    mbc1_write_rom(cart, addr, val);
    break;
  }
}

uint8_t read_ram(Cartridge *cart, const uint16_t addr) {
  if (cart->ram_size > 0) {
    switch (cart->mapper) {
    case ROM_MAPPER:
      // No RAM available
      return 0xFF;

    // MBC1 mappers with built in RAM
    case MBC1_MAPPER:
      return mbc1_read_ram(cart, addr);
    }
  }
  return 0xFF;
}

void write_ram(Cartridge *cart, const uint16_t addr, const uint8_t val) {
  if (cart->ram_size > 0) {
    switch (cart->mapper) {
    // Mappers with no ram.
    case ROM_MAPPER:
      break;

    // MBC1 mappers with RAM
    case MBC1_MAPPER:
      mbc1_write_ram(cart, addr, val);
      break;
    }
  }
}
