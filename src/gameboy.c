#include "gameboy.h"
#include "cartridge.h"
#include "cpu.h"
#include "instructions.h"
#include "interrupt.h"
#include "optables.h"
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

void run_gameboy_loop(struct gameboy *gb) {
  FILE *log_file = fopen("log.txt", "w");
  while (gb->state == GB_RUNNING) {
    int cycles = 0;

    if (gb->cpu.enable_interrupts) {
      gb->cpu.enable_interrupts = false;
      gb->cpu.IME = true;
    }

    gb->opcode = bus_read(gb, gb->cpu.PC);
    log_curr_instr(gb, log_file);
    ++gb->cpu.PC;
    cycles += unprefixed_ins[gb->opcode](gb);

    if (gb->cpu.IME) {
      gb->cpu.IME = false;
      cycles += service_interrupts(gb);
    }
  }
  fclose(log_file);
}

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

int service_interrupts(struct gameboy *gb) {
  const struct interrupts in = gb->interrupt;

  if (in.enable > 0) {
    push_n16(gb, gb->cpu.PC);
    if ((in.flag & 0x1) && (in.enable & 0x1)) {
      gb->cpu.PC = 0x40;
    } else if ((in.flag & 0x2) && (in.enable & 0x2)) {
      gb->cpu.PC = 0x48;
    } else if ((in.flag & 0x4) && (in.enable & 0x4)) {
      gb->cpu.PC = 0x50;
    } else if ((in.flag & 0x8) && (in.enable & 0x8)) {
      gb->cpu.PC = 0x58;
    } else if ((in.flag & 0x10) && (in.enable & 0x10)) {
      gb->cpu.PC = 0x60;
    }
  }

  gb->interrupt.enable = gb->interrupt.flag = 0;
  return 5;
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
