#include "gameboy.h"
#include "cartridge.h"
#include "cpu.h"
#include "optable.h"
#include <stdbool.h>
#include <stdio.h>

bool init_gameboy(GameBoy *gameboy, const char *path_to_rom) {
  gameboy->cartridge = create_cartridge(path_to_rom);
  if (gameboy->cartridge) {
    gameboy->interrupt_enable = false;
    init_cpu(&gameboy->cpu);
    return true;
  } else {
    gameboy->state = GB_STOPPED;
    return false;
  }
}

void run_gameboy(GameBoy *gameboy) {
  while (gameboy->state == GB_RUNNING) {
    const uint8_t opcode = read_byte(gameboy, gameboy->cpu.PC++);

    const Instruction ins = optable[opcode];
    gameboy->cycles = ins.cycles;
    ins.exec(&gameboy->cpu);
  }
}

void close_gameboy(GameBoy *gameboy) {
  if (gameboy) {
    destroy_cartridge(gameboy->cartridge);
  }
}

uint8_t read_byte(GameBoy *gameboy, uint16_t addr) {
  if (0x0000 <= addr && addr <= 0x7FFF) /* ROM */ {
    return read_rom(gameboy->cartridge, addr);
  }

  if (0x8000 <= addr && addr <= 0x9FFF) { // TODO: VRAM
    fprintf(stderr, "Error: Attempt to read VRAM\n");
    gameboy->state = GB_STOPPED;
  }

  if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    return read_ram(gameboy->cartridge, addr);
  }

  if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    return gameboy->wram[addr - 0xC000];
  }

  if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    return gameboy->wram[addr - 0xE000];
  }

  if (0xFE00 <= addr && addr <= 0xFE9F) /* TODO: OAM */ {
    fprintf(stderr, "Error: Attempt to read OAM\n");
    gameboy->state = GB_STOPPED;
  }

  if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to read Prohibited space\n");
    return 0xFF;
  }

  if (0xFF00 <= addr && addr <= 0xFF7F) /* IO Registers */ {
    return read_io(&gameboy->io, addr);
  }

  if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    return gameboy->hram[addr - 0xFF80];
  }

  /* TODO: IE */
  return gameboy->interrupt_enable;
}

void write_byte(GameBoy *gameboy, uint16_t addr, uint8_t val) {
  if (0x0000 <= addr && addr <= 0x7FFF) /* ROM */ {
    write_rom(gameboy->cartridge, addr, val);
    return;
  }

  if (0x8000 <= addr && addr <= 0x9FFF) { // TODO: VRAM
    fprintf(stderr, "Error: Attempt to read VRAM\n");
    gameboy->state = GB_STOPPED;
    return;
  }

  if (0xA000 <= addr && addr <= 0xBFFF) /* EXRAM */ {
    write_ram(gameboy->cartridge, addr, val);
    return;
  }

  if (0xC000 <= addr && addr <= 0xDFFF) /* WRAM */ {
    gameboy->wram[addr - 0xC000] = val;
    return;
  }

  if (0xE000 <= addr && addr <= 0xFDFF) /* Echo RAM */ {
    gameboy->wram[addr - 0xE000] = val;
    return;
  }

  if (0xFE00 <= addr && addr <= 0xFE9F) /* TODO: OAM */ {
    fprintf(stderr, "Error: Attempt to read OAM\n");
    gameboy->state = GB_STOPPED;
    return;
  }

  if (0xFEA0 <= addr && addr <= 0xFEFF) /* Prohibited */ {
    fprintf(stderr, "Warn: Attempt to read Prohibited space\n");
    return;
  }

  if (0xFF00 <= addr && addr <= 0xFF7F) /* IO Registers */ {
    write_io(&gameboy->io, addr, val);
    return;
  }

  if (0xFF80 <= addr && addr <= 0xFFFE) /* HRAM */ {
    gameboy->hram[addr - 0xFF80] = val;
    return;
  }

  /* TODO: IE */
  gameboy->interrupt_enable = val;
}
