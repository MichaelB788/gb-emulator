#include "core/bus.h"
#include <stdint.h>

uint8_t read_byte(Bus *bus, uint16_t addr) { return bus->memory[addr]; }

void write_byte(Bus *bus, uint16_t addr, uint8_t val) {
  bus->memory[addr] = val;
}
