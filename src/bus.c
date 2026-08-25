#include "bus.h"
#include "cartridge.h"
#include "interrupts.h"
#include "joypad.h"
#include "serial.h"
#include "timer.h"
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void bus_init(struct bus *bus, struct cartridge *cart) {
  assert(cart != nullptr);
  bus->cart = cart;
  bus->joypad.JOYP = 0x3F;
}

void bus_tick(struct bus *bus) { timer_tick(&bus->timer, &bus->interrupts); }

uint8_t bus_read_byte(const struct bus *bus, uint16_t addr) {
  if (addr <= 0x7FFF)
    return cartridge_read_rom(bus->cart, addr);

  if (0x8000 <= addr && addr <= 0x9FFF)
    return bus->vram[addr - 0x8000];

  if (0xA000 <= addr && addr <= 0xBFFF)
    return cartridge_read_ram(bus->cart, addr);

  if (0xC000 <= addr && addr <= 0xDFFF)
    return bus->wram[addr - 0xC000];

  if (0xE000 <= addr && addr <= 0xFDFF)
    return bus->wram[addr - 0xE000];

  if (0xFE00 <= addr && addr <= 0xFE9F)
    assert(false && "OAM read");

  if (0xFEA0 <= addr && addr <= 0xFEFF)
    return 0xFF;

  if (0xFF00 <= addr && addr <= 0xFF7F || addr == 0xFFFF)
    return bus_read_io(bus, addr);

  if (0xFF80 <= addr && addr <= 0xFFFE)
    return bus->hram[addr - 0xFF80];

  unreachable();
}

void bus_write_byte(struct bus *bus, uint16_t addr, uint8_t val) {
  if (addr <= 0x7FFF)
    cartridge_write_rom(bus->cart, addr, val);

  else if (0x8000 <= addr && addr <= 0x9FFF)
    bus->vram[addr - 0x8000] = val;

  else if (0xA000 <= addr && addr <= 0xBFFF)
    cartridge_write_ram(bus->cart, addr, val);

  else if (0xC000 <= addr && addr <= 0xDFFF)
    bus->wram[addr - 0xC000] = val;

  else if (0xE000 <= addr && addr <= 0xFDFF)
    bus->wram[addr - 0xE000] = val;

  else if (0xFE00 <= addr && addr <= 0xFE9F)
    assert(false && "OAM write");

  else if (0xFEA0 <= addr && addr <= 0xFEFF)
    return;

  else if (0xFF00 <= addr && addr <= 0xFF7F || addr == 0xFFFF)
    bus_write_io(bus, addr, val);

  else if (0xFF80 <= addr && addr <= 0xFFFE)
    bus->hram[addr - 0xFF80] = val;

  unreachable();
}

uint8_t bus_read_io(const struct bus *bus, uint16_t addr) {
  switch (addr) {
  case 0xFF00:
    return bus->joypad.JOYP;
  case 0xFF01:
    return bus->serial.SB;
  case 0xFF02:
    return bus->serial.SC;
  case 0xFF04:
    return bus->timer.DIV;
  case 0xFF05:
    return bus->timer.TIMA;
  case 0xFF06:
    return bus->timer.TMA;
  case 0xFF07:
    return bus->timer.TAC;
  case 0xFF0F:
    return bus->interrupts.IF;
  case 0xFFFF:
    return bus->interrupts.IE;
  default:
    return 0xFF;
  }
}

void bus_write_io(struct bus *bus, uint16_t addr, uint8_t val) {
  switch (addr) {
  case 0xFF00: {
    const uint8_t written = val & JOYP_MODE_SELECT;
    if (written == JOYP_MODE_SELECT) {
      bus->joypad.JOYP = 0xFF;
    } else {
      bus->joypad.JOYP &= ~JOYP_BUTTONS_SELECT;
      bus->joypad.JOYP |= written;
    }
  } break;
  case 0xFF01:
    bus->serial.SB = val;
    break;
  case 0xFF02:
    bus->serial.SC = val | SC_UNUSED;
    if (val == 0x81) {
      putchar(bus->serial.SB);
      fflush(stdout);
    }
    break;
  case 0xFF04:
    bus->timer.system_counter = 0;
    bus->timer.DIV = 0;
    break;
  case 0xFF05:
    bus->timer.TIMA = val;
    break;
  case 0xFF06:
    bus->timer.TMA = val;
    break;
  case 0xFF07:
    bus->timer.TAC = val & ~TAC_UNUSED;
    break;
  case 0xFF0F:
    bus->interrupts.IF = val & ~INTERRUPT_UNUSED;
    break;
  case 0xFFFF:
    bus->interrupts.IE = val & ~INTERRUPT_UNUSED;
    break;
  default:
    break;
  }
}
