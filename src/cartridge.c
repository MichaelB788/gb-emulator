#include "cartridge.h"
#include "byte_sizes.h"
#include "mbc1.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool init_mapper(struct cartridge *cart) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
    init_mbc1(&cart->mbc1);
    break;
  default:
    fprintf(stderr, "Cannot init unknown mapper %d\n", (int)cart->type);
    return false;
  }
  return true;
}

bool initialize_rom(FILE *rom_file, struct cartridge *cart) {
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

bool initialize_ram(struct cartridge *cart) {
  if (cart->ram_size > 0) {
    cart->ram = malloc(cart->ram_size);
    if (!cart->ram) {
      perror("RAM malloc failed");
      return false;
    }
  }
  return true;
}

bool initialize_cart_from_header(FILE *rom_file, struct cartridge *cart) {
  uint8_t header[0x150] = {0};
  if (fread(header, 1, 0x150, rom_file) != 0x150) {
    perror("Could not read file content into header");
    return false;
  }
  rewind(rom_file);

  cart->type = (enum cartridge_type)header[0x147];
  cart->rom_size = KiB_32 * (1 << header[0x148]);

  // ram_sizes[0x01] is unused by any official ROMs, so it's unknown what RAM
  // size it should correspond to, if any.
  const size_t ram_sizes[] = {0, 0, KiB_8, KiB_32, KiB_128, KiB_64};
  cart->ram_size = ram_sizes[header[0x149]];

  return init_mapper(cart) && initialize_rom(rom_file, cart) &&
         initialize_ram(cart);
}

bool init_cartridge(struct cartridge *cart, const char *path_to_rom) {
  FILE *rom_file = fopen(path_to_rom, "rb");
  if (!rom_file) {
    perror("Could not open file");
    return false;
  }
  if (initialize_cart_from_header(rom_file, cart)) {
    fclose(rom_file);
    return true;
  } else {
    fclose(rom_file);
    destroy_cartridge(cart);
    return false;
  }
}

void destroy_cartridge(struct cartridge *cart) {
  if (cart) {
    free(cart->rom);
    free(cart->ram);
  }
}

uint8_t rom_read(const struct cartridge *cart, const uint16_t addr) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    return cart->rom[addr];
  case MBC1_CART:
    return mbc1_read_rom(cart, addr);
  default:
    fprintf(stderr, "Warn: Unknown mapper type, cannot read ROM\n");
    return 0xFF;
  }
}

void rom_write(struct cartridge *cart, const uint16_t addr, const uint8_t val) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
    mbc1_write_rom(cart, addr, val);
    break;
  default:
    fprintf(stderr, "Warn: Unknown mapper type, cannot write ROM\n");
    break;
  }
}

uint8_t ram_read(struct cartridge *cart, const uint16_t addr) {
  if (cart->ram_size > 0) {
    switch (cart->type) {
    case MBC1_RAM_CART:
    case MBC1_RAM_BATTERY_CART:
      return mbc1_read_ram(cart, addr);
    default:
      break;
    }
  }
  fprintf(stderr, "Warn: Attempt to read RAM that doesn't exist\n");
  return 0xFF;
}

void ram_write(struct cartridge *cart, const uint16_t addr, const uint8_t val) {
  if (cart->ram_size > 0) {
    switch (cart->type) {
    case MBC1_RAM_CART:
    case MBC1_RAM_BATTERY_CART:
      mbc1_write_ram(cart, addr, val);
      break;
    default:
      break;
    }
  }
  fprintf(stderr, "Warn: Attempt to write RAM that doesn't exist\n");
}
