#include "core/mapper.h"

uint16_t internal_rom_addr(uint16_t addr, uint8_t rom_bank) {
  return addr + (rom_bank * 0x4000);
}

uint16_t internal_ram_addr(uint16_t addr, uint8_t ram_bank,
                           size_t max_external_ram_size) {
  // 0xA000 is the start address of exRAM, and this section is 0x2000 (8192)
  // bytes long.
  return ((addr - 0xA000) + (ram_bank * 0x2000)) % max_external_ram_size;
}
