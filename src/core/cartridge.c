#include "core/cartridge.h"
#include "core/mbc.h"

void init_cartridge(Cartridge *cart, const char *path_to_rom) {}

uint8_t cartridge_read(Cartridge *cart, uint16_t addr) {
  switch (cart->mbc.type) {
  case ROM_ONLY:
    return read_rom(&cart->mbc.rom, addr);
  }
}

void cartridge_write(Cartridge *cart, uint16_t addr, uint8_t val) {
  switch (cart->mbc.type) {
  case ROM_ONLY:
    write_rom(&cart->mbc.rom, addr, val);
    break;
  }
}
