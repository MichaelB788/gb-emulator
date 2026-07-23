#include "bus.h"
#include "bitwise.h"
#include "cartridge.h"
#include "mapper.h"
#include "serial_transfer.h"
#include "timer.h"
#include <assert.h>
#include <stdio.h>

bool bus_init(struct bus *bus, const char *rom_path) {
  bus->joypad = 0x3F;

  bus->interrupt.enable = 0;
  bus->interrupt.flag = 0;

  bus->serial.data = 0;
  bus->serial.control = 0;

  bus->timer.elapsed_cycles = 0;
  bus->timer.system_counter = 0;
  bus->timer.divider = 0;
  bus->timer.counter = 0;
  bus->timer.modulo = 0;
  bus->timer.control = 0;

  return cart_init(&bus->cartridge, rom_path);
}

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
    fprintf(stderr, "Error: Attempt to read from OAM\n");
    return 0xFF;
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
    fprintf(stderr, "Warn: Attempt to write to OAM\n");
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
    // All inputs are considered released if no mode is selected
    return (bus->joypad & 0x30) == 0x30 ? 0x3F : bus->joypad;
  case 0xFF01:
    return bus->serial.data;
  case 0xFF02:
    return bus->serial.control;
  case 0xFF04:
    return bus->timer.divider;
  case 0xFF05:
    return bus->timer.counter;
  case 0xFF06:
    return bus->timer.modulo;
  case 0xFF07:
    return bus->timer.control;
  case 0xFF0F:
    return bus->interrupt.flag;
  case 0xFFFF:
    return bus->interrupt.enable;
  default:
    return 0xFF;
  }
}

void bus_write_io(struct bus *bus, uint16_t addr, uint8_t val) {
  switch (addr) {
  case 0xFF00:
    // Lower nibble is read only.
    bus->joypad = (val & 0x30) | (bus->joypad & 0xF);
    break;
  case 0xFF01:
    bus->serial.data = val;
    break;
  case 0xFF02:
    if (val & 0x80) {
      putchar(bus->serial.data);
      fflush(stdout);
    }
    set_bit(&bus->interrupt.flag, 3); // Request a serial interrupt
    break;
  case 0xFF04:
    bus->timer.system_counter = 0;
    bus->timer.divider = 0;
    break;
  case 0xFF05:
    bus->timer.counter = val;
    break;
  case 0xFF06:
    bus->timer.modulo = val;
    break;
  case 0xFF07:
    bus->timer.control = val & 0x7;
    break;
  case 0xFF0F:
    bus->interrupt.flag = val & 0x1F;
    break;
  case 0xFFFF:
    bus->interrupt.enable = val & 0x1F;
    break;
  default:
    break;
  }
}
