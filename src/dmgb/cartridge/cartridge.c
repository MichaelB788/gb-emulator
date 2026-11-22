#include "cartridge.h"
#include "byte_sizes.h"
#include "mbc1.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define HEADER_CART_TYPE 0x147ul
#define HEADER_ROM_SIZE 0x148ul
#define HEADER_RAM_SIZE 0x149ul
#define HEADER_LEN 0x150ul

static bool cartridge_header_is_valid(const uint8_t *header);
static union cartridge_mapper cartridge_init_mapper(uint8_t type);

bool cartridge_create(struct cartridge *cart, const char *rom_path) {
  cartridge_destroy(cart);
  *cart = (struct cartridge){0};

  FILE *rom_file = fopen(rom_path, "rb");
  if (rom_file == NULL) {
    perror("Could not open ROM file");
    return false;
  }

  // read and validate header
  uint8_t header[HEADER_LEN];
  if (fread(header, 1, sizeof header, rom_file) < sizeof header) {
    perror("Could not fread ROM file into header");
    goto cartridge_cleanup;
  }
  if (!cartridge_header_is_valid(header))
    goto cartridge_cleanup;

  // cartridge type
  cart->mapper = cartridge_init_mapper(cart->type = header[HEADER_CART_TYPE]);

  // rom
  cart->rom =
      calloc(cart->rom_size = KIB(32) * (1u << header[HEADER_ROM_SIZE]), 1);
  rewind(rom_file);
  if (fread(cart->rom, 1, cart->rom_size, rom_file) < cart->rom_size) {
    perror("Could not fread ROM file into ROM data");
    goto cartridge_cleanup;
  }
  fclose(rom_file);

  // ram
  const size_t ram_sizes[] = {0, 0, KIB(8), KIB(32), KIB(128), KIB(64)};
  if (header[HEADER_RAM_SIZE] > 1)
    cart->ram = calloc(cart->ram_size = ram_sizes[header[HEADER_RAM_SIZE]], 1);

  return true;

cartridge_cleanup:
  if (rom_file)
    fclose(rom_file);

  cartridge_destroy(cart);
  return false;
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
  case ROM_ONLY_CART: return cart->rom[a16];
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_rom_read(&cart->mapper.mbc1, cart->rom, cart->rom_size, a16);
  default: assert(false);
  }
}

void cartridge_write_rom(struct cartridge *cart, uint16_t a16, uint8_t u8) {
  switch (cart->type) {
  case ROM_ONLY_CART: break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_rom_write(&cart->mapper.mbc1, cart->rom_size, a16, u8);
    break;
  default: assert(false);
  }
}

uint8_t cartridge_read_ram(const struct cartridge *cart, uint16_t a16) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_ram(&cart->mapper.mbc1, cart->ram, cart->ram_size, a16);
  default: return 0xFF;
  }
}

void cartridge_write_ram(struct cartridge *cart, uint16_t a16, uint8_t u8) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_ram(&cart->mapper.mbc1, cart->ram, cart->ram_size, a16, u8);
    break;
  default: break;
  }
}

bool cartridge_header_is_valid(const uint8_t *header) {
  if (header[HEADER_CART_TYPE] > MBC1_RAM_BATTERY_CART) {
    fprintf(stderr, "Unimplemented cartridge type: %d\n",
            header[HEADER_CART_TYPE]);
    return false;
  }

  if (header[HEADER_ROM_SIZE] > 8) {
    fprintf(stderr, "Invalid ROM size byte 0x%02X\n", header[HEADER_ROM_SIZE]);
    return false;
  }

  if (header[HEADER_RAM_SIZE] == 1 || header[HEADER_RAM_SIZE] > 5) {
    fprintf(stderr, "Invalid RAM size byte 0x%02X\n", header[HEADER_RAM_SIZE]);
    return false;
  }

  return true;
}

union cartridge_mapper cartridge_init_mapper(uint8_t type) {
  switch (type) {
  case MBC1_RAM_BATTERY_CART:
  case MBC1_RAM_CART:
  case MBC1_CART: return (union cartridge_mapper){.mbc1 = {.rom_bank = 1}};

  default: return (union cartridge_mapper){0};
  }
}
