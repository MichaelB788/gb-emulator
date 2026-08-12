#include "mapper.h"
#include "vector.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

bool create_mapper(struct mapper *mapper, uint8_t mapper_type) {
  mapper->type = mapper_type;
  switch (mapper_type) {
  case ROM_ONLY_CART:
    return true;
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mapper->mbc1 = create_mbc1();
    return true;
  default:
    fprintf(stderr, "Cannot init unknown mapper %d\n", mapper_type);
    return false;
  }
}

uint8_t mapper_read_rom(const struct mapper *mapper,
                        const struct u8_fixed_vec *rom, uint16_t addr) {
  switch (mapper->type) {
  case ROM_ONLY_CART:
    return rom->data[addr];
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_rom(&mapper->mbc1, rom, addr);
  default:
    assert(false && "mapper_read_rom fail");
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
  default:
    assert(false && "mapper_write_rom fail");
  }
}

uint8_t mapper_read_ram(const struct mapper *mapper,
                        const struct u8_fixed_vec *ram, uint16_t addr) {
  switch (mapper->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    return mbc1_read_ram(&mapper->mbc1, ram, addr);
  default:
    return 0xFF;
  }
}

void mapper_write_ram(const struct mapper *mapper, struct u8_fixed_vec *ram,
                      uint16_t addr, uint8_t val) {
  switch (mapper->type) {
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mbc1_write_ram(&mapper->mbc1, ram, addr, val);
    break;
  default:
    break;
  }
}
