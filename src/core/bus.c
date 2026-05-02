#include "core/bus.h"
#include "core/cartridge.h"
#include <stdint.h>

uint8_t read_byte(Bus *bus, uint16_t addr) {
  if (addr < 0x8000) // ROM
    return read_rom(bus->cartridge, addr);
  if (addr < 0xA000) // TODO: VRAM
    return 0x10;
  if (addr < 0xC000) // External RAM
    return read_ram(bus->cartridge, addr);
  if (addr < 0xE000) // Work RAM (No banking)
    return bus->wram[addr - 0xC000];
  if (addr < 0xFE00) // Echo RAM
    return bus->wram[addr - 0xE000];
  if (addr < 0xFEA0) // TODO: OAM
    return 0x10;
  if (addr < 0xFF00) // Prohibited space
    return 0x10;
  if (addr < 0xFF80) // TODO: IO Registers
    return 0x10;
  if (addr < 0xFFFE) // High RAM
    return bus->hram[addr - 0xFF80];
  else // Interrupt Enable Register
    return bus->interrupt_enable;
}

void write_byte(Bus *bus, uint16_t addr, uint8_t val) {
  if (addr < 0x8000) // ROM
    write_rom(bus->cartridge, addr, val);
  if (addr < 0xA000) // TODO: VRAM
    return;
  if (addr < 0xC000) // External RAM
    write_ram(bus->cartridge, addr, val);
  if (addr < 0xE000) // Work RAM (No banking)
    bus->wram[addr - 0xC000] = val;
  if (addr < 0xFE00) // Echo RAM
    bus->wram[addr - 0xE000] = val;
  if (addr < 0xFEA0) // TODO: OAM
    return;
  if (addr < 0xFF00) // Prohibited space
    return;
  if (addr < 0xFF80) // TODO: IO Registers
    return;
  if (addr < 0xFFFE) // High RAM
    bus->hram[addr - 0xFF80] = val;
  else // Interrupt Enable Register
    bus->interrupt_enable = val & 1;
}
