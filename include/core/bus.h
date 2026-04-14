#pragma once
#include <stdint.h>

typedef struct {
  uint8_t memory[0x10000];
} Bus;

uint8_t read(Bus *bus, uint16_t addr);
