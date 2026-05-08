#include "core/bus.h"
#include "core/cartridge.h"
#include <stdint.h>

enum {
  ROM_END = 0x8000,
  VRAM_END = 0xA000,
  EXRAM_END = 0xC000,
  WRAM_BEGIN = 0xC000,
  WRAM_END = 0xE000,
  ECHO_BEGIN = 0xFE00,
  ECHO_END = 0xFE00,
  OAM_END = 0xFEA0,
  PROHIBITED_END = 0xFF00,
  IO_END = 0xFF80,
  HRAM_BEGIN = 0xFF80,
  HRAM_END = 0xFFFE
};

uint8_t read_byte(Bus *bus, uint16_t addr) {
  if (addr < ROM_END) {
    return read_rom(bus->cartridge, addr);
  }
  if (addr < VRAM_END) {
    // TODO: VRAM
    return 0x10;
  }
  if (addr < EXRAM_END) {
    return read_ram(bus->cartridge, addr);
  }
  if (addr < WRAM_END) {
    return bus->wram[addr - WRAM_BEGIN];
  }
  if (addr < ECHO_END) {
    return bus->wram[addr - ECHO_BEGIN];
  }
  if (addr < OAM_END) {
    // TODO: OAM
    return 0x10;
  }
  if (addr < PROHIBITED_END) {
    return 0x10;
  }
  if (addr < IO_END) {
    // TODO: IO Registers
    return 0x10;
  }
  if (addr < HRAM_END) {
    return bus->hram[addr - HRAM_BEGIN];
  }
  return bus->interrupt_enable;
}

void write_byte(Bus *bus, uint16_t addr, uint8_t val) {
  if (addr < ROM_END) {
    write_rom(bus->cartridge, addr, val);
    return;
  }
  if (addr < VRAM_END) {
    // TODO: VRAM
    return;
  }
  if (addr < EXRAM_END) {
    write_ram(bus->cartridge, addr, val);
    return;
  }
  if (addr < WRAM_END) {
    bus->wram[addr - WRAM_BEGIN] = val;
    return;
  }
  if (addr < ECHO_END) {
    bus->wram[addr - ECHO_BEGIN] = val;
    return;
  }
  if (addr < OAM_END) {
    // TODO: OAM
    return;
  }
  if (addr < PROHIBITED_END) {
    return;
  }
  if (addr < IO_END) {
    // TODO: IO Registers
    return;
  }
  if (addr < HRAM_END) {
    bus->hram[addr - HRAM_BEGIN] = val;
    return;
  }
  bus->interrupt_enable = val;
}
