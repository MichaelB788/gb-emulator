#include "core/bus.h"
#include <stdint.h>

uint8_t read(Bus *bus, uint16_t addr) { return bus->memory[addr]; }
