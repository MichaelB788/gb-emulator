#pragma once
#include "u8_array.h"
#include <stdbool.h>
#include <stdint.h>

struct mbc1 {
  bool is_advanced_banking_enabled;
  bool is_ram_enabled;
  uint8_t rom_bank;
  uint8_t ram_bank;
};

uint8_t mbc1_rom_read(const struct mbc1 *mbc1, const struct u8_array *rom,
                      uint16_t a16);
void mbc1_rom_write(struct mbc1 *mbc1, const struct u8_array *rom, uint16_t a16,
                    uint8_t u8);

uint8_t mbc1_read_ram(const struct mbc1 *mbc1, const struct u8_array *ram,
                      uint16_t a16);
void mbc1_write_ram(const struct mbc1 *mbc1, struct u8_array *ram, uint16_t a16,
                    uint8_t u8);
