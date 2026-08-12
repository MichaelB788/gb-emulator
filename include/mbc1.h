#pragma once
#include <stdbool.h>
#include <stdint.h>

struct u8_fixed_vec;

struct mbc1 {
  uint8_t rom_bank;
  uint8_t ram_bank;
  bool advanced_banking_enabled;
  bool ram_enabled;
};

struct mbc1 create_mbc1();

uint8_t mbc1_read_rom(const struct mbc1 *mbc1, const struct u8_fixed_vec *rom,
                      uint16_t addr);
void mbc1_write_rom(struct mbc1 *mbc1, const struct u8_fixed_vec *rom,
                    uint16_t addr, uint8_t val);

uint8_t mbc1_read_ram(const struct mbc1 *mbc1, const struct u8_fixed_vec *ram,
                      uint16_t addr);
void mbc1_write_ram(const struct mbc1 *mbc1, struct u8_fixed_vec *ram,
                    uint16_t addr, uint8_t val);
