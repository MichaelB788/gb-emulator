#include "cartridge.h"
#include "constants.h"
#include "mbc1.h"
#include "u8_buf.h"
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

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

  switch (header[0x147]) {
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

  static constexpr size_t RAM_CAPS[] = {0ul,    0ul,     KiB_8,
                                        KiB_32, KiB_128, KiB_64};
  cart->type = header[0x147];
  u8_buf_create(&cart->rom, KiB_32 * (1 << header[0x148]));
  u8_buf_create(&cart->ram, RAM_CAPS[header[0x149]]);

  rewind(rom_f);
  fread(cart->rom.data, 1, cart->rom.cap, rom_f);
  if (ferror(rom_f)) {
    perror("cartridge_create");
    fclose(rom_f);
    return false;
  }

  fclose(rom_f);
  return true;
}

void cartridge_destroy(struct cartridge *cart) {
  u8_buf_destroy(&cart->rom);
  u8_buf_destroy(&cart->ram);
}

uint8_t cartridge_read_rom(const struct cartridge *cart, uint16_t addr) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    return cart->rom.data[addr];
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_rom(&cart->mbc1, &cart->rom, addr);
  }
}

uint8_t cartridge_read_ram(const struct cartridge *cart, uint16_t addr) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_ram(&cart->mbc1, &cart->ram, addr);
  default:
    return 0xFF;
  }
}

void cartridge_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val) {
  switch (cart->type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_rom(&cart->mbc1, &cart->rom, addr, val);
    break;
  }
}

void cartridge_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val) {
  switch (cart->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_ram(&cart->mbc1, &cart->ram, addr, val);
    break;
  default:
    break;
  }
}
