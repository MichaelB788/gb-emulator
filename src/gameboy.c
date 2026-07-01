#include "gameboy.h"
#include "cartridge.h"
#include "cpu.h"
#include "interrupt.h"
#include "serial.h"
#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool init_gameboy(struct gameboy *gb, const char *path_to_rom) {
  init_cpu(&gb->cpu);
  return init_cartridge(&gb->cartridge, path_to_rom);
}

void close_gameboy(struct gameboy *gb) { destroy_cartridge(&gb->cartridge); }

uint8_t bus_read(struct gameboy *gb, uint16_t addr) {
  if (0x0000 <= addr && addr <= 0x7FFF) /* ROM */ {
    return mapper_read(&gb->cartridge, addr);
  } else if (0x8000 <= addr && addr <= 0x9FFF) /* VRAM */ {
    return gb->vram[addr - 0x8000];
  } else if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    return exram_read(&gb->cartridge, addr);
  } else if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    return gb->wram[addr - 0xC000];
  } else if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    return gb->wram[addr - 0xE000];
  } else if (0xFE00 <= addr && addr <= 0xFE9F) /* OAM */ {
    fprintf(stderr, "Error: Attempt to read from OAM\n");
    gb->state = GB_STOPPED;
    return 0xFF;
  } else if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to read from prohibited space\n");
    gb->state = GB_STOPPED;
    return 0xFF;
  } else if (0xFF00 <= addr && addr <= 0xFF7F ||
             addr == 0xFFFF) /* IO Registers */ {
    return io_read(gb, addr);
  } else if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    return gb->hram[addr - 0xFF80];
  }

  assert(0 && "impossible bus read");
}

void bus_write(struct gameboy *gb, uint16_t addr, uint8_t val) {
  if (0x0000 <= addr && addr <= 0x7FFF) /* ROM */ {
    mapper_write(&gb->cartridge, addr, val);
  } else if (0x8000 <= addr && addr <= 0x9FFF) /* VRAM */ {
    gb->vram[addr - 0x8000] = val;
  } else if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    exram_write(&gb->cartridge, addr, val);
  } else if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    gb->wram[addr - 0xC000] = val;
  } else if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    gb->wram[addr - 0xE000] = val;
  } else if (0xFE00 <= addr && addr <= 0xFE9F) /* OAM */ {
    fprintf(stderr, "Warn: Attempt to write to OAM\n");
    gb->state = GB_STOPPED;
    return;
  } else if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to write to prohibited space\n");
    gb->state = GB_STOPPED;
  } else if (0xFF00 <= addr && addr <= 0xFF7F ||
             addr == 0xFFFF) /* IO Registers */ {
    io_write(gb, addr, val);
  } else if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    gb->hram[addr - 0xFF80] = val;
  }
}

uint8_t io_read(struct gameboy *gb, uint16_t addr) {
  if (addr == 0xFF00) {
    if ((gb->joypad & 0x30) == 0x30) {
      return 0x3F; // All inputs are considered released if no mode is selected
    } else {
      return gb->joypad;
    }
  } else if (addr == 0xFF01 || addr == 0xFF02) {
    return serial_read(&gb->serial, (enum serial_reg)addr);
  } else if (0xFF04 <= addr && addr <= 0xFF07) {
    return 0xFF; // TODO: Timer read
  } else if (addr == 0xFF0F || addr == 0xFFFF) {
    return interrupt_read(&gb->interrupt, (enum interrupt_reg)addr);
  } else {
    return 0xFF;
  }
}

void io_write(struct gameboy *gb, uint16_t addr, uint8_t val) {
  if (addr == 0xFF00) {
    gb->joypad = val & 0x3; // Lower nibble is read only
  } else if (addr == 0xFF01 || addr == 0xFF02) {
    serial_write(&gb->serial, (enum serial_reg)addr, val);
  } else if (0xFF04 <= addr && addr <= 0xFF07) {
    return; // TODO: Timer write
  } else if (addr == 0xFF0F || addr == 0xFFFF) {
    interrupt_write(&gb->interrupt, (enum interrupt_reg)addr, val);
  }
}
