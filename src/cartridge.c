#include "cartridge.h"
#include "mbc1.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum header_fields {
  HEADER_CARTRIDGE_TYPE = 0x147,
  HEADER_ROM_SIZE = 0x148,
  HEADER_RAM_SIZE = 0x149
};

int cartridge_create(struct cartridge *cart, const char *rom_path) {
  FILE *rom_f = fopen(rom_path, "rb");
  if (rom_f == NULL) {
    perror("cartridge_create fopen");
    goto cart_fail;
  }

  // read header into buffer
  uint8_t header[0x150];
  fread(header, sizeof(uint8_t), 0x150, rom_f);
  if (ferror(rom_f)) {
    perror("cartridge_create fread header");
    goto cart_fail;
  }

  // init cartridge type
  switch (cart->type = header[HEADER_CARTRIDGE_TYPE]) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    cart->mapper.mbc1 = (struct mbc1){.rom_bank = 1};
    break;
  default:
    fprintf(stderr, "cartridge_create unimplemented mapper: 0x%02X\n",
            cart->type);
    goto cart_fail;
  }

  // init RAM
  static const size_t RAM_CAPS[] = {
      [0x0] = 0,       // no ram
      [0x1] = 0,       // unused, true value is unknown
      [0x2] = 0x2000,  // 8 KiB
      [0x3] = 0x8000,  // 32 KiB
      [0x4] = 0x20000, // 128 KiB
      [0x5] = 0x10000  // 64 KiB
  };
  cart->ram = malloc(cart->ram_size = RAM_CAPS[header[HEADER_RAM_SIZE]]);

  // init ROM
  rewind(rom_f);
  cart->rom = malloc(cart->rom_size = 0x8000 * (1 << header[HEADER_ROM_SIZE]));
  fread(cart->rom, sizeof(uint8_t), cart->rom_size, rom_f);
  if (ferror(rom_f)) {
    perror("cartridge_create fread rom");
    goto cart_fail;
  }

  return 0;

cart_fail:
  fclose(rom_f);
  cartridge_destroy(cart);
  return -1;
}

void cartridge_destroy(struct cartridge *cart) {
  cart->rom_size = cart->ram_size = 0;
  if (cart->rom) {
    free(cart->rom);
    cart->rom = NULL;
  }
  if (cart->ram) {
    free(cart->ram);
    cart->ram = NULL;
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
