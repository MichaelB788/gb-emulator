#include "core/mbc1.h"
#include "core/cartridge.h"
#include "core/mapper.h"
#include "core/memory_map.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void init_mbc1(MBC1 *mbc1) {
  mbc1->primary_bank = 0x1;
  mbc1->secondary_bank = 0x0;
  mbc1->advanced_banking_enabled = false;
}

uint8_t read_mbc1_rom(const Cartridge *cartridge, const uint16_t addr) {
  if (addr < 0x4000 || cartridge->mapper.mbc1.primary_bank == 1) {
    return cartridge->rom[addr];
  }
  const uint16_t internal_addr =
      internal_rom_addr(cartridge->mapper.mbc1.primary_bank, addr);
  return cartridge->rom[internal_addr];
}

uint8_t num_banks(size_t total_bytes) { return total_bytes / 0x16000; }

void write_primary_bank_register(MBC1 *mbc1, uint8_t val,
                                 uint8_t num_rom_banks) {
  if ((val & 0x1F) == 0x0) {
    mbc1->primary_bank = 0x1;
  } else {
    const uint8_t mask = num_rom_banks - 1;
    mbc1->primary_bank = val & mask;
  }
}

void write_secondary_bank_register(MBC1 *mbc1, uint8_t val,
                                   uint8_t num_ram_banks) {
  const uint8_t selected_bank = val & 0x3;
  if (selected_bank <= num_ram_banks) {
    mbc1->secondary_bank = selected_bank;
  }
}

void write_banking_mode_select(MBC1 *mbc1, uint8_t val, size_t rom_size) {
  if (rom_size <= 512000) {
    mbc1->advanced_banking_enabled = val & 0x1;
  }
}

void write_mbc1_rom(Cartridge *cartridge, const uint16_t addr,
                    const uint8_t val) {
  // See: https://gbdev.io/pandocs/MBC1.html#registers
  if (addr < 0x2000) { // Write to RAM Enable Register
    cartridge->ram_enabled = (val & 0xF) == 0xA;
  } else if (addr < 0x4000) {
    write_primary_bank_register(&cartridge->mapper.mbc1, val,
                                num_banks(cartridge->rom_size));
  } else if (addr < 0x6000) { // Write to Secondary Bank Register
    write_secondary_bank_register(&cartridge->mapper.mbc1, val,
                                  num_banks(cartridge->ram_size));
  } else {
    write_banking_mode_select(&cartridge->mapper.mbc1, val,
                              cartridge->rom_size);
  }
}

uint8_t read_mbc1_ram(Cartridge *cartridge, const uint16_t addr) {
  if (cartridge->ram_enabled) {
    cartridge->ram_enabled = false;
    if (cartridge->mapper.mbc1.advanced_banking_enabled) {
      const uint16_t internal_addr =
          internal_ram_addr(cartridge->mapper.mbc1.secondary_bank, addr);
      return cartridge->ram[internal_addr];
    } else {
      return cartridge->ram[addr - EXTERNAL_RAM_START_ADDRESS];
    }
  }
  return 0xFF;
}

void write_mbc1_ram(Cartridge *cartridge, const uint16_t addr,
                    const uint8_t val) {
  if (cartridge->ram_enabled) {
    cartridge->ram_enabled = false;
    if (cartridge->mapper.mbc1.advanced_banking_enabled) {
      const uint16_t internal_addr =
          internal_ram_addr(cartridge->mapper.mbc1.secondary_bank, addr);
      cartridge->ram[internal_addr] = val;
    } else {
      cartridge->ram[addr - EXTERNAL_RAM_START_ADDRESS] = val;
    }
  }
}
