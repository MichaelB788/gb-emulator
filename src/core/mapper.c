#include "core/mapper.h"
#include "core/memory_map.h"
#include <stddef.h>
#include <stdint.h>

uint16_t internal_ram_addr(const uint8_t ram_bank, const uint16_t addr) {
  const size_t initial_offset = addr - EXTERNAL_RAM_START_ADDRESS;
  const size_t bank_offset = ram_bank * EXTERNAL_RAM_SIZE;

  // The MBC will wrap around the internal RAM address should it be OOB.
  return (initial_offset + bank_offset) % EXTERNAL_RAM_SIZE;
}
