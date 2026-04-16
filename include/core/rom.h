#pragma once
#include <assert.h>
#include <stdint.h>

typedef struct {
  uint8_t memory[32000];
} ROM;

static inline uint8_t read_rom(ROM *rom, uint16_t addr) {
  assert(addr < 32000);
  return rom->memory[addr];
}

static inline void write_rom(ROM *rom, uint16_t addr, uint8_t val) {
  (void)val;
}
