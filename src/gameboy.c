#include "gameboy.h"
#include "bitwise.h"
#include "cartridge.h"
#include "cpu.h"
#include "interrupts.h"
#include "serial_transfer.h"
#include "timer.h"
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
    return rom_read(&gb->cartridge, addr);
  } else if (0x8000 <= addr && addr <= 0x9FFF) /* VRAM */ {
    return gb->vram[addr - 0x8000];
  } else if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    return ram_read(&gb->cartridge, addr);
  } else if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    return gb->wram[addr - 0xC000];
  } else if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    return gb->wram[addr - 0xE000];
  } else if (0xFE00 <= addr && addr <= 0xFE9F) /* OAM */ {
    fprintf(stderr, "Error: Attempt to read from OAM\n");
    return 0xFF;
  } else if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to read from prohibited space\n");
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
    rom_write(&gb->cartridge, addr, val);
  } else if (0x8000 <= addr && addr <= 0x9FFF) /* VRAM */ {
    gb->vram[addr - 0x8000] = val;
  } else if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    ram_write(&gb->cartridge, addr, val);
  } else if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    gb->wram[addr - 0xC000] = val;
  } else if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    gb->wram[addr - 0xE000] = val;
  } else if (0xFE00 <= addr && addr <= 0xFE9F) /* OAM */ {
    fprintf(stderr, "Warn: Attempt to write to OAM\n");
    return;
  } else if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to write to prohibited space\n");
  } else if (0xFF00 <= addr && addr <= 0xFF7F ||
             addr == 0xFFFF) /* IO Registers */ {
    io_write(gb, addr, val);
  } else if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    gb->hram[addr - 0xFF80] = val;
  }
}

uint8_t io_read(struct gameboy *gb, uint16_t addr) {
  switch (addr) {
  case 0xFF00:
    if ((gb->joypad & 0x30) == 0x30) {
      return 0x3F; // All inputs are considered released if no mode is selected
    } else {
      return gb->joypad;
    }
    break;
  case 0xFF01:
    return gb->serial.data;
  case 0xFF02:
    return gb->serial.control;
  case 0xFF04:
    return gb->timer.divider;
  case 0xFF05:
    return gb->timer.counter;
  case 0xFF06:
    return gb->timer.modulo;
  case 0xFF07:
    return gb->timer.control;
  case 0xFF0F:
    return gb->interrupt.flag;
  case 0xFFFF:
    return gb->interrupt.enable;
  default:
    return 0xFF;
  }
}

void io_write(struct gameboy *gb, uint16_t addr, uint8_t val) {
  switch (addr) {
  case 0xFF00:
    // Lower nibble is read only.
    gb->joypad = (val & 0x30) | (gb->joypad & 0xF);
    break;
  case 0xFF01:
    gb->serial.data = val;
    break;
  case 0xFF02:
    if (val & 0x80) {
      putchar(gb->serial.data);
      fflush(stdout);
    }
    set_bit(&gb->interrupt.flag, 3); // Request a serial interrupt
    break;
  case 0xFF04:
    gb->timer.system_counter = 0;
    gb->timer.divider = 0;
    break;
  case 0xFF05:
    gb->timer.counter = val;
    break;
  case 0xFF06:
    gb->timer.modulo = val;
    break;
  case 0xFF07:
    gb->timer.control = val & 0x7;
    break;
  case 0xFF0F:
    gb->interrupt.flag = val & 0x1F;
    break;
  case 0xFFFF:
    gb->interrupt.enable = val & 0x1F;
    break;
  }
}

void log_curr_instr(struct gameboy *gb, FILE *output) {
  const struct cpu *cpu = &gb->cpu;
  fprintf(output,
          "A:%02X F:%02X B:%02X C:%02X D:%02X E:%02X H:%02X L:%02X SP:%04X "
          "PC:%04X PCMEM:%02X,%02X,%02X,%02X\n",
          cpu->A, cpu->F, cpu->B, cpu->C, cpu->D, cpu->E, cpu->H, cpu->L,
          cpu->SP, cpu->PC, bus_read(gb, cpu->PC), bus_read(gb, cpu->PC + 1),
          bus_read(gb, cpu->PC + 2), bus_read(gb, cpu->PC + 3));
  fflush(output);
}
