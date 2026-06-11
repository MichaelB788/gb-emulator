#include "mbc1.h"
#include "byte_sizes.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void init_mbc1(MBC1 *mbc1) {
  mbc1->rom_bank = 1;
  mbc1->ram_bank = 0;
  mbc1->advanced_banking_enabled = false;
}

uint8_t read_mbc1_rom(const MBC1 *mbc1, const uint16_t addr) {
  if (mbc1->rom_bank <= 1) {
    return mbc1->rom[addr];
  } else {
    return mbc1->rom[addr + (KiB_16 * mbc1->rom_bank)];
  }
}

void write_primary_bank_register(MBC1 *mbc1, uint8_t val) {
  // TODO
  if ((val & 0x1F) == 0x0) {
    mbc1->rom_bank = 1;
  } else {
    // Mask out the bits that are not needed.
    const uint8_t bits_needed = mbc1->rom_size / KiB_16;
    if (bits_needed > 5) {
      if (mbc1->advanced_banking_enabled) {
        mbc1->rom_bank = (mbc1->ram_bank << 5) + (val & 0x1F);
      } else {
        mbc1->rom_bank = val & 0x1F;
      }
    } else {
      mbc1->rom_bank = val & ((1 << bits_needed) - 1);
    }
  }
}

void write_secondary_bank_register(MBC1 *mbc1, uint8_t val) {
  const uint8_t selected_bank = val & 0x3;
  if (selected_bank <= num_ram_banks) {
    mbc1->secondary_bank = selected_bank;
  }
}

void write_banking_mode_select(MBC1 *mbc1, uint8_t val, size_t rom_size) {
  // Advanced banking only has an affect on cartridges with ROM sizes greater
  // than 512 KiB.
  if (rom_size <= KiB_512) {
    mbc1->advanced_banking_enabled = val & 0x1;
  }
}

void write_mbc1_rom(MBC1 *mbc1, const uint16_t addr, const uint8_t val) {
  // See: https://gbdev.io/pandocs/MBC1.html#registers
  if (addr < 0x2000) {
    mbc1->ram_enabled = (val & 0xF) == 0xA;
  } else if (addr < 0x4000) {
    write_primary_bank_register(mbc1, val);
  } else if (addr < 0x6000) {
    write_secondary_bank_register(mbc1, val);
  } else {
    write_banking_mode_select(mbc1, val, mbc1->rom_size);
  }
}

uint8_t read_mbc1_ram(const MBC1 *cartridge, const uint16_t addr) {
  if (cartridge->ram_enabled) {
    if (cartridge->mbc1.advanced_banking_enabled) {
      const uint16_t internal_addr = internal_ram_addr(
          addr, cartridge->mbc1.secondary_bank, cartridge->ram_size);
      return cartridge->ram[internal_addr];
    } else {
      return cartridge->ram[addr - 0xA000];
    }
  }
  return 0xFF;
}

void write_mbc1_ram(MBC1 *cartridge, const uint16_t addr, const uint8_t val) {
  if (cartridge->ram_enabled) {
    if (cartridge->mbc1.advanced_banking_enabled) {
      const uint16_t internal_addr = internal_ram_addr(
          addr, cartridge->mbc1.secondary_bank, cartridge->ram_size);
      cartridge->ram[internal_addr] = val;
    } else {
      cartridge->ram[addr - 0xA000] = val;
    }
  }
}
