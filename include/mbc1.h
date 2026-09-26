#pragma once
#include <stdint.h>

struct cartridge;

struct mbc1 {
  bool advanced_banking_enabled;
  uint8_t rom_bank;
  uint8_t ram_bank;
  bool ram_enabled;
};

void mbc1_init(struct mbc1 *mbc);

[[nodiscard]] uint8_t cartridge_mbc1_read_rom(const struct cartridge *cart,
                                              uint16_t a16);
[[nodiscard]] uint8_t cartridge_mbc1_read_ram(const struct cartridge *cart,
                                              uint16_t a16);

void cartridge_mbc1_write_rom(struct cartridge *cart, uint16_t a16, uint8_t u8);
void cartridge_mbc1_write_ram(struct cartridge *cart, uint16_t a16, uint8_t u8);
