#include "mapper.h"
#include "mbc1.h"
#include "vector.h"
#include <assert.h>
#include <stdint.h>

struct mapper mapper_create(enum mapper_type type) {
  struct mapper m = {.type = type};
  switch (type) {
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    m.mbc1 = mbc1_create();
    break;
  default:
    break;
  }
  return m;
}

uint8_t mapper_read_rom(struct mapper mapper, const struct u8_fixed_vec *rom,
                        uint16_t addr) {
  switch (mapper.type) {
  case ROM_ONLY_CART:
    return rom->data[addr];
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_rom(&mapper.mbc1, rom, addr);
  }
}

void mapper_write_rom(struct mapper *mapper, const struct u8_fixed_vec *rom,
                      uint16_t addr, uint8_t val) {
  switch (mapper->type) {
  case ROM_ONLY_CART:
    break;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_rom(&mapper->mbc1, rom, addr, val);
    break;
  }
}

uint8_t mapper_read_ram(struct mapper mapper, const struct u8_fixed_vec *ram,
                        uint16_t addr) {
  switch (mapper.type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_ram(&mapper.mbc1, ram, addr);
  default:
    return 0xFF;
  }
}

void mapper_write_ram(struct mapper mapper, struct u8_fixed_vec *ram,
                      uint16_t addr, uint8_t val) {
  switch (mapper.type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_ram(&mapper.mbc1, ram, addr, val);
    break;
  default:
    break;
  }
}
