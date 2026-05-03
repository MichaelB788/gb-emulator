#pragma once
#include <stddef.h>
#include <stdint.h>

uint16_t internal_rom_addr(uint16_t addr, uint8_t rom_bank);

uint16_t internal_ram_addr(uint16_t addr, uint8_t ram_bank,
                           size_t max_external_ram_size);
