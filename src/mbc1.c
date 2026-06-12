#include "mbc1.h"
#include "byte_sizes.h"
#include "cartridge.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void init_mbc1(MBC1 *mbc1) {
  mbc1->rom_bank = 1;
  mbc1->ram_bank = 0;
  mbc1->advanced_banking_enabled = false;
  mbc1->ram_enabled = false;
}

uint8_t mbc1_read_rom(const Cartridge *cart, const uint16_t addr) {
  if (cart->mbc1.rom_bank <= 1) {
    return cart->rom[addr];
  } else {
    return cart->rom[addr + (KiB_16 * cart->mbc1.rom_bank)];
  }
}

void mbc1_write_rom(Cartridge *cart, const uint16_t addr, const uint8_t val) {
  // See: https://gbdev.io/pandocs/MBC1.html#registers
  if (addr <= 0x1FFF) /* RAM Enable */ {
    cart->mbc1.ram_enabled = (val & 0xF) == 0xA;
  } else if (0x2000 <= addr && addr <= 0x3FFF) /* ROM Bank Number */ {
    const uint8_t bits_needed = cart->rom_size / KiB_16;
    if (bits_needed > 5) {
      if (cart->mbc1.advanced_banking_enabled) {
        cart->mbc1.rom_bank = (cart->mbc1.ram_bank << 5) + val & 0x1F;
      } else {
        cart->mbc1.rom_bank = val & 0x1F;
      }
    } else {
      cart->mbc1.rom_bank = val & ((1 << bits_needed) - 1);
    }
    if ((cart->mbc1.rom_bank & 0x1F) == 0) {
      cart->mbc1.rom_bank += 1;
    }
  } else if (0x4000 <= addr && addr <= 0x5FFF) /* RAM Bank Number */ {
    if (cart->ram_size >= KiB_32 || cart->rom_size >= MiB_1) {
      cart->mbc1.ram_bank = val & 0x3;
    }
  } else if (0x6000 <= addr && addr <= 0x7FFF) /* Banking Mode Select */ {
    cart->mbc1.advanced_banking_enabled = val & 1;
  }
}

uint8_t mbc1_read_ram(const Cartridge *cart, const uint16_t addr) {
  if (cart->mbc1.ram_enabled) {
    const size_t internal_address =
        ((addr - 0xA000) + (cart->mbc1.ram_bank * KiB_8)) % cart->ram_size;
    return cart->ram[internal_address];
  } else {
    return 0xFF;
  }
}

void mbc1_write_ram(Cartridge *cart, const uint16_t addr, const uint8_t val) {
  if (cart->mbc1.ram_enabled) {
    const size_t internal_address =
        ((addr - 0xA000) + (cart->mbc1.ram_bank * KiB_8)) % cart->ram_size;
    cart->ram[internal_address] = val;
  }
}
