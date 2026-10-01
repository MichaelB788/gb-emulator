#include "bus.h"
#include "cartridge.h"
#include "ppu.h"
#include "serial_transfer.h"
#include "timer.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

void bus_init(struct bus *bus, struct cartridge *cart) {
  assert(cart != nullptr);
  bus->cartridge = cart;
  bus->joypad = 0x3F;
  bus->serial_transfer.control = bus->serial_transfer.data = 0;
  bus->interrupt.flag = bus->interrupt.enable = 0;
  ppu_init(&bus->ppu);
  timer_init(&bus->timer);
}

void bus_tick(struct bus *bus) { timer_tick(&bus->timer, &bus->interrupt); }

[[nodiscard]] static uint8_t bus_read_io(const struct bus *bus, uint16_t a16) {
  switch (a16) {
    // clang-format off
  case 0xFF00: return bus->joypad & 0x30 ? bus->joypad : 0x3F;
  case 0xFF01: return bus->serial_transfer.data;
  case 0xFF02: return bus->serial_transfer.control;
  case 0xFF04: return bus->timer.system_counter >> 8 & 0xFF;
  case 0xFF05: return bus->timer.counter;
  case 0xFF06: return bus->timer.modulo;
  case 0xFF07: return bus->timer.control;
  case 0xFF0F: return bus->interrupt.flag;
  case 0xFF40: return bus->ppu.lcd.control;
  case 0xFF41: return bus->ppu.lcd.status;
  case 0xFF44: return bus->ppu.lcd.y_coordinate;
  case 0xFF45: return bus->ppu.lcd.compare;
  default: return 0xFF;
    // clang-format on
  }
}

static void bus_write_io(struct bus *bus, uint16_t a16, uint8_t u8) {
  switch (a16) {
    // clang-format off
  case 0xFF00: bus->joypad = bus->joypad & 0xCF | u8 & 0x30; break; // Lower nibble read only
  case 0xFF01: bus->serial_transfer.data = u8; break;
  case 0xFF02: serial_transfer_write_control(&bus->serial_transfer, u8); break;
  case 0xFF04: bus->timer.system_counter = 0; break;
  case 0xFF05: bus->timer.counter = u8; break;
  case 0xFF06: bus->timer.modulo = u8; break;
  case 0xFF07: bus->timer.control = u8 & 0x7; break;
  case 0xFF0F: bus->interrupt.flag = u8 & 0x1F; break;
  case 0xFF40: bus->ppu.lcd.control = u8;
  case 0xFF41: bus->ppu.lcd.status = u8 & 0x7F;
  case 0xFF44: bus->ppu.lcd.y_coordinate = u8;
  case 0xFF45: bus->ppu.lcd.compare = bus->ppu.lcd.compare & 0x7 | u8 & 0x78; // Lower 3-bits read only
  default: break;
    // clang-format on
  }
}

uint8_t bus_read(const struct bus *bus, uint16_t a16) {
  // clang-format off
  if (0x0000 <= a16 && a16 <= 0x7FFF) return cartridge_read_rom(bus->cartridge, a16);
  if (0x8000 <= a16 && a16 <= 0x9FFF) return ppu_read_vram(&bus->ppu, a16);
  if (0xA000 <= a16 && a16 <= 0xBFFF) return cartridge_read_ram(bus->cartridge, a16);
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
  if (0x0000 <= a16 && a16 <= 0x7FFF) cartridge_write_rom(bus->cartridge, a16, u8);
  else if (0x8000 <= a16 && a16 <= 0x9FFF) ppu_write_vram(&bus->ppu, a16, u8);
  else if (0xA000 <= a16 && a16 <= 0xBFFF) cartridge_write_ram(bus->cartridge, a16, u8);
  else if (0xC000 <= a16 && a16 <= 0xDFFF) bus->wram[a16 - 0xC000] = u8;
  else if (0xE000 <= a16 && a16 <= 0xFDFF) bus->wram[a16 - 0xE000] = u8;
  else if (0xFE00 <= a16 && a16 <= 0xFE9F) ppu_write_oam(&bus->ppu, a16, u8);
  else if (0xFEA0 <= a16 && a16 <= 0xFEFF) return;
  else if (0xFF00 <= a16 && a16 <= 0xFF7F) bus_write_io(bus, a16, u8);
  else if (0xFF80 <= a16 && a16 <= 0xFFFE) bus->hram[a16 - 0xFF80] = u8;
  else bus->interrupt.enable = u8 & 0x1F;
  // clang-format on
}
