#include "bus.h"
#include "cartridge.h"
#include "joypad.h"
#include "lcd.h"
#include "ppu.h"
#include "serial_transfer.h"
#include "timer.h"
#include <stdint.h>

int bus_create(struct bus *bus, const char *rom_path) {
  bus->joypad.p1 = 0xFF;
  ppu_init(&bus->ppu);

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
  case 0xFF00: return joypad_read(&bus->joypad);
  case 0xFF01: return bus->serial.sb;
  case 0xFF02: return bus->serial.sc;
  case 0xFF04: return bus->timer.sys >> 8 & 0xFF; // DIV is the upper byte of SYS
  case 0xFF05: return bus->timer.tima;
  case 0xFF06: return bus->timer.tma;
  case 0xFF07: return bus->timer.tac;
  case 0xFF0F: return bus->interrupt.flag;
  case 0xFF40: return bus->ppu.lcd.lcdc;
  case 0xFF41: return bus->ppu.lcd.stat;
  case 0xFF42: return bus->ppu.lcd.scy;
  case 0xFF43: return bus->ppu.lcd.scx;
  case 0xFF44: return bus->ppu.lcd.ly;
  case 0xFF45: return bus->ppu.lcd.lyc;
  case 0xFF47: return bus->ppu.lcd.bgp;
  case 0xFF48: return bus->ppu.lcd.obp0;
  case 0xFF49: return bus->ppu.lcd.obp1;
  default: return 0xFF;
    // clang-format on
  }
}

static void bus_write_io(struct bus *bus, uint16_t a16, uint8_t u8) {
  switch (a16) {
    // clang-format off
  case 0xFF00: joypad_write(&bus->joypad, u8); break;
  case 0xFF01: bus->serial.sb = u8; break;
  case 0xFF02: serial_transfer_write_control(&bus->serial, u8); break;
  case 0xFF04: bus->timer.sys = 0; break; // writing to DIV resets SYS to 0
  case 0xFF05: bus->timer.tima = u8; break;
  case 0xFF06: bus->timer.tma = u8; break;
  case 0xFF07: bus->timer.tac = u8 & 0x7; break;
  case 0xFF0F: bus->interrupt.flag = u8 & 0x1F; break;
  case 0xFF40: bus->ppu.lcd.lcdc = u8; break;
  case 0xFF41: lcd_write_stat(&bus->ppu.lcd, u8); break;
  case 0xFF42: bus->ppu.lcd.scy = u8; break;
  case 0xFF43: bus->ppu.lcd.scx = u8; break;
  case 0xFF44: bus->ppu.lcd.ly = u8; break;
  case 0xFF45: bus->ppu.lcd.lyc = u8; break;
  case 0xFF47: bus->ppu.lcd.bgp = u8; break;
  case 0xFF48: bus->ppu.lcd.obp0 = u8; break;
  case 0xFF49: bus->ppu.lcd.obp1 = u8; break;
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
  else return bus->interrupt.enable;
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
  else bus->interrupt.enable = u8 & 0x1F;
  // clang-format on
}
