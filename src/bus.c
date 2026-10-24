#include "bus.h"
#include "cartridge.h"
#include "ppu.h"
#include "serial_transfer.h"
#include "timer.h"
#include <stdint.h>

int bus_create(struct bus *bus, const char *rom_path) {
  bus->joypad.P1 = 0xFF;

  return cartridge_create(&bus->cartridge, rom_path);
}

void bus_destroy(struct bus *bus) { cartridge_destroy(&bus->cartridge); }

void bus_tick(struct bus *bus) {
  ppu_tick(&bus->ppu, &bus->interrupt);
  timer_tick(&bus->timer, &bus->interrupt);
}

static uint8_t bus_read_io(const struct bus *bus, uint16_t a16) {
  switch (a16) {
    // clang-format off
  case 0xFF00: return bus->joypad.P1 & 0x30 ? bus->joypad.P1 : 0xFF;
  case 0xFF01: return bus->serial.SB;
  case 0xFF02: return bus->serial.SC;
  case 0xFF04: return bus->timer.SYS >> 8 & 0xFF;
  case 0xFF05: return bus->timer.TIMA;
  case 0xFF06: return bus->timer.TMA;
  case 0xFF07: return bus->timer.TAC;
  case 0xFF0F: return bus->interrupt.IF;
  case 0xFF40: return bus->ppu.LCDC;
  case 0xFF41: return bus->ppu.STAT;
  case 0xFF44: return bus->ppu.LY;
  case 0xFF45: return bus->ppu.LYC;
  case 0xFF47: return bus->ppu.BGP;
  default: return 0xFF;
    // clang-format on
  }
}

static void bus_write_io(struct bus *bus, uint16_t a16, uint8_t u8) {
  switch (a16) {
    // clang-format off
  case 0xFF00: bus->joypad.P1 = bus->joypad.P1 & 0xCF | u8 & 0x30; break; // Lower nibble read only
  case 0xFF01: bus->serial.SB = u8; break;
  case 0xFF02: serial_transfer_write_control(&bus->serial, u8); break;
  case 0xFF04: bus->timer.SYS = 0; break;
  case 0xFF05: bus->timer.TIMA = u8; break;
  case 0xFF06: bus->timer.TMA = u8; break;
  case 0xFF07: bus->timer.TAC = u8 & 0x7; break;
  case 0xFF0F: bus->interrupt.IF = u8 & 0x1F; break;
  case 0xFF40: bus->ppu.LCDC = u8;
  case 0xFF41: bus->ppu.STAT = bus->ppu.STAT & 0x7 | u8 & 0x78; // Lower 3-bits read only
  case 0xFF44: bus->ppu.LY = u8;
  case 0xFF45: bus->ppu.LYC = u8;
  case 0xFF47: bus->ppu.BGP = u8;
  default: break;
    // clang-format on
  }
}

uint8_t bus_read(const struct bus *bus, uint16_t a16) {
  // clang-format off
  if (0x0000 <= a16 && a16 <= 0x7FFF) return cartridge_read_rom(&bus->cartridge, a16);
  if (0x8000 <= a16 && a16 <= 0x9FFF) return ppu_read_vram(&bus->ppu, a16);
  if (0xA000 <= a16 && a16 <= 0xBFFF) return cartridge_read_ram(&bus->cartridge, a16);
  if (0xC000 <= a16 && a16 <= 0xDFFF) return bus->wram[a16 - 0xC000];
  if (0xE000 <= a16 && a16 <= 0xFDFF) return bus->wram[a16 - 0xE000];
  if (0xFE00 <= a16 && a16 <= 0xFE9F) return ppu_read_oam(&bus->ppu, a16);
  if (0xFEA0 <= a16 && a16 <= 0xFEFF) return 0xFF;
  if (0xFF00 <= a16 && a16 <= 0xFF7F) return bus_read_io(bus, a16);
  if (0xFF80 <= a16 && a16 <= 0xFFFE) return bus->hram[a16 - 0xFF80];
  else return bus->interrupt.IE;
  // clang-format on
}

void bus_write(struct bus *bus, uint16_t a16, uint8_t u8) {
  // clang-format off
  if (0x0000 <= a16 && a16 <= 0x7FFF) cartridge_write_rom(&bus->cartridge, a16, u8);
  else if (0x8000 <= a16 && a16 <= 0x9FFF) ppu_write_vram(&bus->ppu, a16, u8);
  else if (0xA000 <= a16 && a16 <= 0xBFFF) cartridge_write_ram(&bus->cartridge, a16, u8);
  else if (0xC000 <= a16 && a16 <= 0xDFFF) bus->wram[a16 - 0xC000] = u8;
  else if (0xE000 <= a16 && a16 <= 0xFDFF) bus->wram[a16 - 0xE000] = u8;
  else if (0xFE00 <= a16 && a16 <= 0xFE9F) ppu_write_oam(&bus->ppu, a16, u8);
  else if (0xFEA0 <= a16 && a16 <= 0xFEFF) return;
  else if (0xFF00 <= a16 && a16 <= 0xFF7F) bus_write_io(bus, a16, u8);
  else if (0xFF80 <= a16 && a16 <= 0xFFFE) bus->hram[a16 - 0xFF80] = u8;
  else bus->interrupt.IE = u8 & 0x1F;
  // clang-format on
}
