#include "cartridge.h"
#include "mbc1.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool cartridge_create(struct cartridge *cart, const char *rom_path) {
  FILE *rom_f = fopen(rom_path, "rb");
  if (rom_f == nullptr) {
    perror("cartridge_create");
    return false;
  }

  uint8_t header[0x150] = {0};
  fread(header, 1, 0x150, rom_f);
  if (ferror(rom_f)) {
    perror("cartridge_create");
    fclose(rom_f);
    return false;
  }

  uint8_t cart_type = header[0x147];
  switch (cart_type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_init(&cart->mbc1);
    break;
  default:
    fprintf(stderr, "cartridge_create: Unknown mapper 0x%02X", header[0x147]);
    fclose(rom_f);
    return false;
  }

  static constexpr size_t RAM_CAPS[] = {0,         0,          8 * 1024,
                                        32 * 1024, 128 * 1024, 64 * 1024};
  cart->type = (enum cartridge_type)cart_type;
  cart->rom = malloc(cart->rom_size = 32 * 1024 * (1 << header[0x148]));
  cart->ram = malloc(cart->ram_size = RAM_CAPS[header[0x149]]);

  rewind(rom_f);
  fread(cart->rom, 1, cart->rom_size, rom_f);
  if (ferror(rom_f)) {
    perror("cartridge_create");
    fclose(rom_f);
    return false;
  }

  fclose(rom_f);
  return true;
}

void cartridge_destroy(struct cartridge *cart) {
  cart->rom_size = cart->ram_size = 0;
  if (cart->rom) {
    free(cart->rom);
    cart->rom = nullptr;
  }
  if (cart->ram) {
    free(cart->ram);
    cart->ram = nullptr;
  }
}

uint8_t cartridge_read_rom(const struct cartridge *cart, uint16_t a16) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    return cart->rom[a16];
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return cartridge_mbc1_read_rom(cart, a16);
  }
}

uint8_t cartridge_read_ram(const struct cartridge *cart, uint16_t a16) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return cartridge_mbc1_read_ram(cart, a16);
  default:
    return 0xFF;
  }
}

void cartridge_write_rom(struct cartridge *cart, uint16_t a16, uint8_t u8) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    cartridge_mbc1_write_rom(cart, a16, u8);
    break;
  }
}

void cartridge_write_ram(struct cartridge *cart, uint16_t a16, uint8_t u8) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    cartridge_mbc1_write_ram(cart, a16, u8);
    break;
  default:
    break;
  }
}
