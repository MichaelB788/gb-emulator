#include "mbc1.h"
#include "cartridge.h"
#include <stddef.h>
#include <stdint.h>

void mbc1_init(struct mbc1 *mbc) {
  mbc->rom_bank = 1;
  mbc->ram_bank = 0;
  mbc->advanced_banking_enabled = mbc->ram_enabled = false;
}

uint8_t cartridge_mbc1_read_rom(const struct cartridge *cart,
                                const uint16_t a16) {
  return a16 < 0x4000 ? cart->rom[a16]
                      : cart->rom[a16 - 0x4000 + cart->mbc1.rom_bank * 0x4000];
}

uint8_t cartridge_mbc1_read_ram(const struct cartridge *cart,
                                const uint16_t a16) {
  return cart->mbc1.ram_enabled
             ? cart->ram[a16 - 0xA000 + cart->mbc1.ram_bank * 0x2000]
             : 0xFF;
}

// See: https://gbdev.io/pandocs/MBC1.html#registers
void cartridge_mbc1_write_rom(struct cartridge *cart, uint16_t a16,
                              uint8_t u8) {
  if (a16 <= 0x1FFF) {
    cart->mbc1.ram_enabled = (u8 & 0xF) == 0xA;
  } else if (0x2000 <= a16 && a16 <= 0x3FFF) {
    const uint8_t mask = (cart->rom_size / 0x4000) - 1;
    if (mask > 0x1F) {
      cart->mbc1.rom_bank = cart->mbc1.advanced_banking_enabled
                                ? cart->mbc1.ram_bank << 5 | u8 & 0x1F
                                : u8 & 0x1F;
    } else {
      cart->mbc1.rom_bank = u8 & mask;
    }

    if ((cart->mbc1.rom_bank & 0x1F) == 0)
      ++cart->mbc1.rom_bank;
  } else if (0x4000 <= a16 && a16 <= 0x5FFF) {
    cart->mbc1.ram_bank = u8 & 0x3;
  } else if (0x6000 <= a16 && a16 <= 0x7FFF) {
    cart->mbc1.advanced_banking_enabled = u8;
  }
}

void cartridge_mbc1_write_ram(struct cartridge *cart, uint16_t a16,
                              uint8_t u8) {
  if (cart->mbc1.ram_enabled)
    cart->ram[a16 - 0xA000 + cart->mbc1.ram_bank * 0x2000] = u8;
}
