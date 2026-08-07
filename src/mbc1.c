#include "mbc1.h"
#include "constants.h"
#include "vector.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

void mbc1_init(struct mbc1 *mbc1) {
  mbc1->rom_bank = 1;
  mbc1->ram_bank = 0;
  mbc1->advanced_banking_enabled = false;
  mbc1->ram_enabled = false;
}

uint8_t mbc1_read_rom(const struct mbc1 *mbc1, const struct u8_fixed_vec *rom,
                      const uint16_t addr) {
  return addr < 0x4000 ? rom->data[addr]
                       : rom->data[(KiB_16 * mbc1->rom_bank) + (addr - 0x4000)];
}

// See: https://gbdev.io/pandocs/MBC1.html#registers
void mbc1_write_rom(struct mbc1 *mbc1, const struct u8_fixed_vec *rom,
                    const uint16_t addr, const uint8_t val) {
  if (addr <= 0x1FFF) {
    mbc1->ram_enabled = (val & 0xF) == 0xA;
  } else if (0x2000 <= addr && addr <= 0x3FFF) {
    const uint8_t bits_needed = rom->capacity / KiB_16;
    if (bits_needed > 5) {
      const uint8_t bank = val & 0x1F;
      mbc1->rom_bank =
          mbc1->advanced_banking_enabled ? (mbc1->ram_bank << 5) | bank : bank;
    } else {
      mbc1->rom_bank = val & ((1 << bits_needed) - 1);
    }

    if ((mbc1->rom_bank & 0x1F) == 0) {
      ++mbc1->rom_bank;
    }
  } else if (0x4000 <= addr && addr <= 0x5FFF) {
    mbc1->ram_bank = val & 0x3;
  } else if (0x6000 <= addr && addr <= 0x7FFF) {
    mbc1->advanced_banking_enabled = val & 1;
  }
}

uint8_t mbc1_read_ram(const struct mbc1 *mbc1, const struct u8_fixed_vec *ram,
                      const uint16_t addr) {
  return ram->capacity > 0 && mbc1->ram_enabled
             ? ram->data[(addr - 0xA000) + (mbc1->ram_bank * KiB_8)]
             : 0xFF;
}

void mbc1_write_ram(const struct mbc1 *mbc1, struct u8_fixed_vec *ram,
                    const uint16_t addr, const uint8_t val) {
  if (ram->capacity > 0 && mbc1->ram_enabled) {
    ram->data[(addr - 0xA000) + (mbc1->ram_bank * KiB_8)] = val;
  }
}
