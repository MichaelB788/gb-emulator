#include "core/mbc.h"
#include "core/cartridge.h"
#include "core/memory_map.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

uint8_t read_mbc1_rom(const Cartridge *cartridge, const uint16_t addr) {
  if (cartridge->primary_bank < 2 || addr < 0x4000) {
    return cartridge->rom[addr];
  }
  return cartridge->rom[addr + (0x4000 * cartridge->primary_bank)];
}

uint8_t num_banks(size_t total_bytes) { return total_bytes / 0x16000; }

void write_mbc1_rom(Cartridge *cartridge, const uint16_t addr,
                    const uint8_t val) {
  // See: https://gbdev.io/pandocs/MBC1.html#registers
  if (addr < 0x2000) { // Write to RAM Enable Register
    cartridge->ram_enabled = (val & 0xF) == 0xA;
  } else if (addr < 0x4000) { // Write to Primary Bank Register
    if ((val & 0x1F) == 0x0) {
      cartridge->primary_bank = 0x1;
    } else {
      const uint8_t mask = num_banks(cartridge->rom_size) - 1;
      cartridge->primary_bank = val & mask;
    }
  } else if (addr < 0x6000) { // Write to Secondary Bank Register
    const uint8_t selected_bank = val & 0x3;
    if (selected_bank <= num_banks(cartridge->ram_size)) {
      cartridge->secondary_bank = selected_bank;
    }
  } else {
    if (cartridge->rom_size <= 512000) { // Write to Banking Mode Select
      cartridge->advanced_banking_enabled = val & 0x1;
    }
  }
}

uint16_t internal_ram_addr(Cartridge *cartridge, const uint16_t addr) {
  return ((addr - EXTERNAL_RAM_START_ADDRESS) +
          (cartridge->secondary_bank * EXTERNAL_RAM_SIZE)) %
         EXTERNAL_RAM_SIZE;
}

uint8_t read_mbc1_ram(Cartridge *cartridge, const uint16_t addr) {
  if (cartridge->ram_enabled) {
    cartridge->ram_enabled = false;
    if (cartridge->advanced_banking_enabled) {
      return cartridge->ram[internal_ram_addr(cartridge, addr)];
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
    if (cartridge->advanced_banking_enabled) {
      cartridge->ram[internal_ram_addr(cartridge, addr)] = val;
    } else {
      cartridge->ram[addr - EXTERNAL_RAM_START_ADDRESS] = val;
    }
  }
}
