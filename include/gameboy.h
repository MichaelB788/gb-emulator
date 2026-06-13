#pragma once
#include "byte_sizes.h"
#include "cartridge.h"
#include "cpu.h"
#include "io.h"
#include <stdint.h>

typedef enum { GB_RUNNING, GB_STOPPED, GB_HALTED } GameBoyState;

typedef struct GameBoy {
  CPU cpu;
  IO io;
  bool interrupt_enable;
  GameBoyState state;
  uint8_t cycles;
  uint8_t wram[KiB_8];
  uint8_t hram[127];
  Cartridge *cartridge;
} GameBoy;

bool init_gameboy(GameBoy *gameboy, const char *path_to_rom);

void run_gameboy(GameBoy *gameboy);

void close_gameboy(GameBoy *gameboy);

uint8_t read_byte(GameBoy *gameboy, uint16_t addr);

void write_byte(GameBoy *gameboy, uint16_t addr, uint8_t val);

uint16_t read_word(GameBoy *gameboy, uint16_t addr);
