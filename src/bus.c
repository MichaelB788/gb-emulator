#include "bus.h"
#include "cartridge.h"
#include "interrupts.h"
#include "joypad.h"
#include "mapper.h"
#include "serial.h"
#include "timer.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>

bool bus_init(struct bus *bus, const char *rom_path) {
  // NOTE: On real hardware, unused bits are turned into 1 due to pull up
  // resistors
  bus->joypad.JOYP = 0xFF;

  bus->interrupt.IE = bus->interrupt.IF = INTERRUPT_UNUSED;

  bus->serial.SB = 0;
  bus->serial.SC = SC_UNUSED;

  bus->timer.elapsed_cycles = 0;
  bus->timer.system_counter = 0;
  bus->timer.DIV = bus->timer.TIMA = bus->timer.TMA = 0;
  bus->timer.TAC = TAC_UNUSED;

  return cart_init(&bus->cartridge, rom_path);
}

void bus_tick(struct bus *bus) { timer_tick(&bus->timer, &bus->interrupt); }

void bus_close(struct bus *bus) { cart_close(&bus->cartridge); }

uint8_t bus_read_byte(const struct bus *bus, uint16_t addr) {
  if (addr <= 0x7FFF) /* ROM */ {
    return mapper_read_rom(&bus->cartridge, addr);
  }
  if (0x8000 <= addr && addr <= 0x9FFF) /* VRAM */ {
    return bus->vram[addr - 0x8000];
  }
  if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    return mapper_read_ram(&bus->cartridge, addr);
  }
  if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    return bus->wram[addr - 0xC000];
  }
  if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    return bus->wram[addr - 0xE000];
  }
  if (0xFE00 <= addr && addr <= 0xFE9F) /* OAM */ {
    assert(false && "Error: Attempt to read OAM\n");
  }
  if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to read from prohibited space\n");
    return 0xFF;
  }
  if (0xFF00 <= addr && addr <= 0xFF7F || addr == 0xFFFF) /* IO Registers */ {
    return bus_read_io(bus, addr);
  }
  if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    return bus->hram[addr - 0xFF80];
  }
  assert(0 && "impossible bus read");
}

void bus_write_byte(struct bus *bus, uint16_t addr, uint8_t val) {
  if (addr <= 0x7FFF) /* ROM */ {
    mapper_write_rom(&bus->cartridge, addr, val);
  } else if (0x8000 <= addr && addr <= 0x9FFF) /* VRAM */ {
    bus->vram[addr - 0x8000] = val;
  } else if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    mapper_write_ram(&bus->cartridge, addr, val);
  } else if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    bus->wram[addr - 0xC000] = val;
  } else if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    bus->wram[addr - 0xE000] = val;
  } else if (0xFE00 <= addr && addr <= 0xFE9F) /* OAM */ {
    assert(false && "Error: Attempt to write OAM\n");
  } else if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to write to prohibited space\n");
  } else if (0xFF00 <= addr && addr <= 0xFF7F ||
             addr == 0xFFFF) /* IO Registers */ {
    bus_write_io(bus, addr, val);
  } else if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    bus->hram[addr - 0xFF80] = val;
  }
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
    return bus->interrupt.IF;
  case 0xFFFF:
    return bus->interrupt.IE;
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
    if ((bus->serial.SC & SC_TRANSFER_ENABLE) != 0) {
      putchar(bus->serial.SB);
      fflush(stdout);
    }
    bus->interrupt.IF |= INTERRUPT_SERIAL;
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
    bus->timer.TAC = val | TAC_UNUSED;
    break;
  case 0xFF0F:
    bus->interrupt.IF = val | INTERRUPT_UNUSED;
    break;
  case 0xFFFF:
    bus->interrupt.IE = val | INTERRUPT_UNUSED;
    break;
  default:
    break;
  }
}
