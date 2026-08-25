#include "mapper.h"
#include "mbc1.h"
#include <assert.h>
#include <stdint.h>

void mapper_create(struct mapper *mapper, enum mapper_type type) {
  mapper->type = type;
  switch (type) {
  case MBC1_CART:
  case MBC1_RAM_CART:
  case MBC1_RAM_BATTERY_CART:
    mapper->mbc1 = mbc1_init();
    break;
  default:
    break;
  }
}

uint8_t mapper_read_rom(struct mapper mapper, const struct u8_buf *rom,
                        uint16_t addr) {}

void mapper_write_rom(struct mapper *mapper, const struct u8_buf *rom,
                      uint16_t addr, uint8_t val) {}

uint8_t mapper_read_ram(struct mapper mapper, const struct u8_buf *ram,
                        uint16_t addr) {}

void mapper_write_ram(struct mapper mapper, struct u8_buf *ram, uint16_t addr,
                      uint8_t val) {}
