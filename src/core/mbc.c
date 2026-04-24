#include "core/mbc.h"
#include <stdint.h>

uint8_t read_mbc1_rom(Cartridge *cartridge, uint16_t addr) {
  return cartridge->rom[addr + (0x4000 * cartridge->rom_bank)];
}

void write_mbc1_rom(Cartridge *cartridge, uint16_t addr, uint8_t val);

uint8_t read_mbc1_ram(Cartridge *cartridge, uint16_t addr) {
  // TODO: Bank switching
  if (cartridge->ram_enabled) {
    return cartridge->ram[addr];
  } else {
    return 0x00;
  }
}

void write_mbc1_ram(Cartridge *cartridge, uint16_t addr, uint8_t val) {
  // TODO: Bank switching
  if (cartridge->ram_enabled) {
    cartridge->ram[addr] = val;
  }
}
