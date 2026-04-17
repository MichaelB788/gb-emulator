#include "core/bus.h"
#include "core/cartridge.h"
#include <stdint.h>

uint8_t read_byte(Bus *bus, uint16_t addr) {
  if (0 <= addr && addr <= 0x7FFF)
    return read_rom(bus->cartridge, addr);
  else
    return 0x10;
}

void write_byte(Bus *bus, uint16_t addr, uint8_t val) {
  if (0 <= addr && addr <= 0x7FFF)
    write_rom(bus->cartridge, addr, val);
}
