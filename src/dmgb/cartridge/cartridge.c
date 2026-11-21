#include "cartridge.h"
#include "mbc1.h"
#include "u8_array.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

struct cartridge_header {
  uint8_t cartridge_type; // 0x147
  size_t rom_size;        // 0x148
  size_t ram_size;        // 0x149
};

static bool cartridge_is_valid_type(uint8_t cart_type) {
  // If more MBC's are added, this will need to be updated
  return cart_type <= 0x03;
}

static bool cartridge_create_header(struct cartridge_header *header,
                                    FILE *rom_file) {
  header->cartridge_type = header->rom_size = header->ram_size = 0;

  uint8_t u8_buffer[0x150];
  fread(u8_buffer, sizeof(uint8_t), 0x150, rom_file);
  if (feof(rom_file)) {
    perror("cartridge_create_header feof");
    return false;
  }
  if (ferror(rom_file)) {
    perror("cartridge_create_header fread");
    return false;
  }

  if (!cartridge_is_valid_type(header->cartridge_type = u8_buffer[0x147])) {
    fprintf(stderr, "cartridge_create_header: unimplemented type\n");
    return false;
  }

  const uint8_t rom_size_byte = u8_buffer[0x148];
  if (rom_size_byte > 8) {
    fprintf(stderr, "cartridge_create_header: unknown ROM size byte 0x%02X\n",
            rom_size_byte);
    return false;
  }
  header->rom_size = (32 * 1024) * (1 << rom_size_byte);

  const uint8_t ram_size_byte = u8_buffer[0x149];
  switch (ram_size_byte) {
    // clang-format off
  case 0: header->ram_size = 0; break;
  case 2: header->ram_size = 8 * 1024; break;
  case 3: header->ram_size = 32 * 1024; break;
  case 4: header->ram_size = 128 * 1024; break;
  case 5: header->ram_size = 64 * 1024; break;
  // clang-format on
  default:
    fprintf(stderr, "cartridge_create_header: unknown RAM size byte 0x%02X\n",
            ram_size_byte);
    return false;
  }

  return true;
}

static bool cartridge_create_rom(struct u8_array *rom, size_t rom_size,
                                 FILE *rom_file) {
  if (rom_file == NULL) {
    fprintf(stderr, "cartridge_create_rom: rom_file is NULL\n");
    return false;
  }

  if (!u8_array_create(rom, rom_size))
    return false;

  fread(rom->data, sizeof(uint8_t), rom_size, rom_file);
  if (feof(rom_file)) {
    perror("cartridge_create_rom feof");
    return false;
  }
  if (ferror(rom_file)) {
    perror("cartridge_create_rom fread");
    return false;
  }

  return true;
}

static void cartridge_init_mapper(struct cartridge *cart,
                                  enum cartridge_type type) {
  switch (cart->type = type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    cart->mapper.mbc1 = (struct mbc1){.rom_bank = 1};
    break;
  }
}

bool cartridge_create(struct cartridge *cart, const char *rom_path) {
  FILE *rom_file = fopen(rom_path, "rb");
  if (rom_file == NULL) {
    perror("cartridge_create fopen");
    goto cartridge_cleanup;
  }

  struct cartridge_header header;
  if (cartridge_create_header(&header, rom_file)) {
    rewind(rom_file);
    if (!cartridge_create_rom(&cart->rom, header.rom_size, rom_file))
      goto cartridge_cleanup;

    if (!u8_array_create(&cart->ram, header.ram_size))
      goto cartridge_cleanup;

    cartridge_init_mapper(cart, header.cartridge_type);
  } else {
    goto cartridge_cleanup;
  }

  fclose(rom_file);
  return true;

cartridge_cleanup:
  if (rom_file)
    fclose(rom_file);

  cartridge_destroy(cart);
  return false;
}

void cartridge_destroy(struct cartridge *cart) {
  u8_array_destroy(&cart->rom);
  u8_array_destroy(&cart->ram);
}

uint8_t cartridge_read_rom(const struct cartridge *cart, uint16_t a16) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    return cart->rom.data[a16];
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_rom_read(&cart->mapper.mbc1, &cart->rom, a16);
  }
}

void cartridge_write_rom(struct cartridge *cart, uint16_t a16, uint8_t u8) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_rom_write(&cart->mapper.mbc1, &cart->rom, a16, u8);
    break;
  }
}

uint8_t cartridge_read_ram(const struct cartridge *cart, uint16_t a16) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_ram(&cart->mapper.mbc1, &cart->ram, a16);
  default:
    return 0xFF;
  }
}

void cartridge_write_ram(struct cartridge *cart, uint16_t a16, uint8_t u8) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_ram(&cart->mapper.mbc1, &cart->ram, a16, u8);
    break;
  default:
    break;
  }
}
