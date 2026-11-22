#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

struct mbc1 {
  bool advanced_banking_enabled;
  bool ram_enabled;
  uint8_t rom_bank;
  uint8_t ram_bank;
};

uint8_t mbc1_rom_read(const struct mbc1 *mbc1, const uint8_t *rom,
                      size_t rom_size, uint16_t a16);
void mbc1_rom_write(struct mbc1 *mbc1, size_t rom_size, uint16_t a16,
                    uint8_t u8);

uint8_t mbc1_read_ram(const struct mbc1 *mbc1, const uint8_t *ram,
                      size_t ram_size, uint16_t a16);
void mbc1_write_ram(const struct mbc1 *mbc1, uint8_t *ram, size_t ram_size,
                    uint16_t a16, uint8_t u8);
