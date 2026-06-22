#include "gameboy.h"
#include "cartridge.h"
#include "cpu.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool init_gameboy(struct gameboy *gb, const char *path_to_rom) {
  gb->cartridge = create_cartridge(path_to_rom);
  if (gb->cartridge) {
    gb->interrupt_enable = false;
    init_cpu(&gb->cpu);
    return true;
  } else {
    gb->state = GB_STOPPED;
    return false;
  }
}

void close_gameboy(struct gameboy *gb) {
  if (gb) {
    destroy_cartridge(gb->cartridge);
  }
}

uint8_t read_byte(struct gameboy *gb, uint16_t addr) {
  if (0x0000 <= addr && addr <= 0x7FFF) /* ROM */ {
    return read_rom(gb->cartridge, addr);
  }

  if (0x8000 <= addr && addr <= 0x9FFF) { // TODO: VRAM
    return gb->vram[addr - 0x8000];
  }

  if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    return read_ram(gb->cartridge, addr);
  }

  if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    return gb->wram[addr - 0xC000];
  }

  if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    return gb->wram[addr - 0xE000];
  }

  if (0xFE00 <= addr && addr <= 0xFE9F) /* TODO: OAM */ {
    fprintf(stderr, "Error: Attempt to read OAM\n");
    gb->state = GB_STOPPED;
    return 0xFF;
  }

  if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to read Prohibited space\n");
    return 0xFF;
  }

  if (0xFF00 <= addr && addr <= 0xFF7F) /* IO Registers */ {
    return read_io(&gb->io, addr);
  }

  if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    return gb->hram[addr - 0xFF80];
  }

  /* TODO: IE */
  return gb->interrupt_enable;
}

void write_byte(struct gameboy *gb, uint16_t addr, uint8_t val) {
  if (0x0000 <= addr && addr <= 0x7FFF) /* ROM */ {
    write_rom(gb->cartridge, addr, val);
    return;
  }

  if (0x8000 <= addr && addr <= 0x9FFF) { // TODO: VRAM
    gb->vram[addr - 0x8000] = val;
    return;
  }

  if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    write_ram(gb->cartridge, addr, val);
    return;
  }

  if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    gb->wram[addr - 0xC000] = val;
    return;
  }

  if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    gb->wram[addr - 0xE000] = val;
    return;
  }

  if (0xFE00 <= addr && addr <= 0xFE9F) /* TODO: OAM */ {
    fprintf(stderr, "Error: Attempt to read OAM\n");
    gb->state = GB_STOPPED;
    return;
  }

  if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to read Prohibited space\n");
    return;
  }

  if (0xFF00 <= addr && addr <= 0xFF7F) /* IO Registers */ {
    write_io(&gb->io, addr, val);
    return;
  }

  if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    gb->hram[addr - 0xFF80] = val;
    return;
  }

  /* TODO: IE */
  gb->interrupt_enable = val;
}
