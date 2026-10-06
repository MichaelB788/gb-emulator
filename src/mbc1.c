#include "mbc1.h"
#include "cartridge.h"
#include <stddef.h>
#include <stdint.h>

uint8_t cartridge_mbc1_read_rom(const struct cartridge *cart,
                                const uint16_t a16) {
  return a16 < 0x4000
             ? cart->rom[a16]
             : cart->rom[a16 - 0x4000 + cart->mapper.mbc1.rom_bank * 0x4000];
}

uint8_t cartridge_mbc1_read_ram(const struct cartridge *cart,
                                const uint16_t a16) {
  return cart->mapper.mbc1.ram_enable == 0xA
             ? cart->ram[a16 - 0xA000 + cart->mapper.mbc1.ram_bank * 0x2000]
             : 0xFF;
}

// See: https://gbdev.io/pandocs/MBC1.html#registers
void cartridge_mbc1_write_rom(struct cartridge *cart, uint16_t a16,
                              uint8_t u8) {
  if (a16 <= 0x1FFF) {
    cart->mapper.mbc1.ram_enable = u8 & 0xF;
  } else if (0x2000 <= a16 && a16 <= 0x3FFF) {
    const uint8_t mask = (cart->rom_size / 0x4000) - 1;
    if (mask > 0x1F) {
      cart->mapper.mbc1.rom_bank =
          cart->mapper.mbc1.banking_mode_select == 0
              ? u8 & 0x1F
              : cart->mapper.mbc1.ram_bank << 5 | u8 & 0x1F;
    } else {
      cart->mapper.mbc1.rom_bank = u8 & mask;
    }

    if ((cart->mapper.mbc1.rom_bank & 0x1F) == 0)
      ++cart->mapper.mbc1.rom_bank;
  } else if (0x4000 <= a16 && a16 <= 0x5FFF) {
    cart->mapper.mbc1.ram_bank = u8 & 0x3;
  } else if (0x6000 <= a16 && a16 <= 0x7FFF) {
    cart->mapper.mbc1.banking_mode_select = u8 & 1;
  }
}

void cartridge_mbc1_write_ram(struct cartridge *cart, uint16_t a16,
                              uint8_t u8) {
  if (cart->mapper.mbc1.ram_enable == 0xA)
    cart->ram[a16 - 0xA000 + cart->mapper.mbc1.ram_bank * 0x2000] = u8;
}
