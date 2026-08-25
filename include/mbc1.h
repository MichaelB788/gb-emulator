#pragma once
#include <stdint.h>

struct u8_buf;

enum banking_mode { BANKING_SIMPLE = 0, BANKING_ADVANCED = 1 };

struct mbc1 {
  enum banking_mode mode;
  uint8_t rom_bank;
  uint8_t ram_bank;
  bool ram_enabled;
};

void mbc1_create(struct mbc1 *mbc);

[[nodiscard]] uint8_t mbc1_read_rom(const struct mbc1 *mbc1,
                                    const struct u8_buf *rom, uint16_t addr);

[[nodiscard]] uint8_t mbc1_read_ram(const struct mbc1 *mbc1,
                                    const struct u8_buf *ram, uint16_t addr);

void mbc1_write_rom(struct mbc1 *mbc1, const struct u8_buf *rom, uint16_t addr,
                    uint8_t val);

void mbc1_write_ram(const struct mbc1 *mbc1, struct u8_buf *ram, uint16_t addr,
                    uint8_t val);
