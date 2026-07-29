#include "mapper.h"
#include "cartridge.h"
#include <assert.h>
#include <stdint.h>

bool mapper_init(struct cartridge *cart, uint8_t mapper_header_byte) {
  cart->mapper = (enum mapper)mapper_header_byte;
  switch (mapper_header_byte) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_init(&cart->mbc1);
    break;
  default:
    fprintf(stderr, "Cannot init unknown mapper %d\n", mapper_header_byte);
    return false;
  }
  return true;
}

uint8_t mapper_read_rom(const struct cartridge *cart, uint16_t addr) {
  switch (cart->mapper) {
  case ROM_ONLY_CART:
    return cart->rom.data[addr];
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_rom(&cart->mbc1, &cart->rom, addr);
  default:
    assert(false && "mapper_read_rom fail");
  }
}

void mapper_write_rom(struct cartridge *cart, uint16_t addr, uint8_t val) {
  switch (cart->mapper) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_rom(&cart->mbc1, &cart->rom, addr, val);
    break;
  default:
    assert(false && "mapper_write_rom fail");
  }
}

uint8_t mapper_read_ram(const struct cartridge *cart, uint16_t addr) {
  switch (cart->mapper) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_ram(&cart->mbc1, &cart->ram, addr);
  default:
    return 0xFF;
  }
}

void mapper_write_ram(struct cartridge *cart, uint16_t addr, uint8_t val) {
  switch (cart->mapper) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_ram(&cart->mbc1, &cart->ram, addr, val);
    break;
  default:
    break;
  }
}
