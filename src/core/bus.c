#include "core/bus.h"
#include "core/cartridge.h"
#include "core/io.h"
#include <stdint.h>
#include <stdio.h>

uint8_t read_byte(Bus *bus, uint16_t addr) {
  if (0x0000 <= addr && addr <= 0x7FFF) { // ROM
    return read_rom(bus->cartridge, addr);
  } else if (0x8000 <= addr && addr <= 0x9FFF) { // TODO: VRAM
    fprintf(stderr, "Error: Attempt to read VRAM\n");
    return 0x10;
  } else if (0xA000 <= addr && addr <= 0xBFFF) { // EXRAM
    return read_ram(bus->cartridge, addr);
  } else if (0xC000 <= addr && addr <= 0xDFFF) { // WRAM
    return bus->wram[addr - 0xC000];
  } else if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
    return bus->wram[addr - 0xE000];
  } else if (0xFE00 <= addr && addr <= 0xFE9F) { // TODO: OAM
    fprintf(stderr, "Error: Attempt to read OAM\n");
    return 0x10;
  } else if (0xFEA0 <= addr && addr <= 0xFEFF) { // Prohibited
    return 0x10;
  } else if (0xFF00 <= addr && addr <= 0xFF7F) { // IO Registers
    return read_io(&bus->io, addr);
  } else if (0xFF80 <= addr && addr <= 0xFFFE) { // HRAM
    return bus->hram[addr - 0xFF80];
  } else { // IE
    return bus->interrupt_enable;
  }
}

void write_byte(Bus *bus, uint16_t addr, uint8_t val) {
  if (0x0000 <= addr && addr <= 0x7FFF) { // ROM
    write_rom(bus->cartridge, addr, val);
  } else if (0x8000 <= addr && addr <= 0x9FFF) { // TODO: VRAM
    fprintf(stderr, "Error: Attempt to read VRAM\n");
    return;
  } else if (0xA000 <= addr && addr <= 0xBFFF) { // EXRAM
    write_ram(bus->cartridge, addr, val);
  } else if (0xC000 <= addr && addr <= 0xDFFF) { // WRAM
    bus->wram[addr - 0xC000] = val;
  } else if (0xE000 <= addr && addr <= 0xFDFF) { // Echo RAM
    bus->wram[addr - 0xE000] = val;
  } else if (0xFE00 <= addr && addr <= 0xFE9F) { // TODO: OAM
    fprintf(stderr, "Error: Attempt to read OAM\n");
    return;
  } else if (0xFEA0 <= addr && addr <= 0xFEFF) { // Prohibited
    return;
  } else if (0xFF00 <= addr && addr <= 0xFF7F) { // TODO: IO Registers
    return;
  } else if (0xFF80 <= addr && addr <= 0xFFFE) { // HRAM
    bus->hram[addr - 0xFF80] = val;
  } else { // IE
    bus->interrupt_enable = val;
  }
}
