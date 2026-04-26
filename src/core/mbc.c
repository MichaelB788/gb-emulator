#include "core/mbc.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint8_t read_mbc1_rom(const Cartridge *cartridge, const uint16_t addr) {
  if (cartridge->rom_bank < 2 || addr < 0x4000) {
    return cartridge->rom[addr];
  }
  return cartridge->rom[addr + (0x4000 * cartridge->rom_bank)];
}

void write_mbc1_rom(Cartridge *cartridge, const uint16_t addr,
                    const uint8_t val) {
  if (addr < 0x2000) { // Enable RAM Register
    // Any byte with a lower nibble of 0b1010 will enable RAM.
    cartridge->ram_enabled = (val & 0xF) == 0xA;
  } else if (addr < 0x4000) { // ROM Bank Register
    /**
     * If the ROM Bank Number (val) is set to a higher value than the number of
     * banks in the cart, mask only the required bits. See:
     * https://gbdev.io/pandocs/MBC1.html#20003fff--rom-bank-number-write-only
     */
    const uint8_t mask = (cartridge->rom_size / 0x16000) - 1;
    cartridge->rom_bank = val & mask;
    if (cartridge->rom_bank == 0x0) {
      cartridge->rom_bank = 0x1;
    }
  }
}

uint8_t read_mbc1_ram(Cartridge *cartridge, const uint16_t addr) {
  if (cartridge->ram_enabled) {
    const uint16_t external_ram_start_address = 0xA000;
    const uint16_t ram_bank_size = 0x1000;
    const uint16_t internal_addr = ((addr - external_ram_start_address) +
                                    (cartridge->ram_bank * ram_bank_size)) %
                                   ram_bank_size;
    cartridge->ram_enabled = false;
    return cartridge->ram[internal_addr];
  }
  return 0xFF;
}

void write_mbc1_ram(Cartridge *cartridge, const uint16_t addr,
                    const uint8_t val) {
  if (cartridge->ram_enabled) {
    const uint16_t external_ram_start_address = 0xA000;
    const uint16_t ram_bank_size = 0x1000;
    const uint16_t internal_addr = ((addr - external_ram_start_address) +
                                    (cartridge->ram_bank * ram_bank_size)) %
                                   ram_bank_size;
    cartridge->ram[internal_addr] = val;
    cartridge->ram_enabled = false;
  }
}
