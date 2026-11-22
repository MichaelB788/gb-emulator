#include "mbc1.h"
#include "byte_sizes.h"
#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static void mbc1_rom_bank_write(struct mbc1 *mbc1, uint8_t u8, size_t rom_size);

uint8_t mbc1_rom_read(const struct mbc1 *mbc1, const uint8_t *rom,
                      size_t rom_size, uint16_t a16) {
  if (a16 >= 0x4000) {
    const size_t bank_addr = (a16 - 0x4000) + (mbc1->rom_bank * KIB(16));
    assert(bank_addr < rom_size);
    return rom[bank_addr];
  } else {
    return rom[a16];
  }
}

void mbc1_rom_write(struct mbc1 *mbc1, size_t rom_size, uint16_t a16,
                    uint8_t u8) {
  switch (a16 >> 13 & 0x3) {
  case 0: mbc1->ram_enabled = (u8 & 0xF) == 0xA; break;
  case 1: mbc1_rom_bank_write(mbc1, u8, rom_size); break;
  case 2: mbc1->ram_bank = u8 & 0x3; break;
  case 3: mbc1->advanced_banking_enabled = u8 & 1; break;
  default: assert(false);
  }
}

uint8_t mbc1_read_ram(const struct mbc1 *mbc1, const uint8_t *ram,
                      size_t ram_size, uint16_t a16) {
  if (mbc1->ram_enabled && ram_size > 0) {
    const size_t bank_addr = a16 - 0xA000 + mbc1->ram_bank * KIB(8);
    assert(bank_addr < ram_size);
    return ram[bank_addr];
  } else {
    return 0xFF;
  }
}

void mbc1_write_ram(const struct mbc1 *mbc1, uint8_t *ram, size_t ram_size,
                    uint16_t a16, uint8_t u8) {
  if (mbc1->ram_enabled && ram_size > 0) {
    const size_t bank_addr = a16 - 0xA000 + mbc1->ram_bank * KIB(8);
    assert(bank_addr < ram_size);
    ram[bank_addr] = u8;
  }
}

void mbc1_rom_bank_write(struct mbc1 *mbc1, uint8_t u8, size_t rom_size) {
  const uint8_t mask = (rom_size / KIB(16)) - 1;
  if (mask > 0x1F) {
    if (mbc1->advanced_banking_enabled)
      mbc1->rom_bank = mbc1->ram_bank << 5 | u8 & 0x1F;
    else
      mbc1->rom_bank = u8 & 0x1F;
  } else {
    mbc1->rom_bank = u8 & mask;
  }

  if ((mbc1->rom_bank & 0x1F) == 0)
    ++mbc1->rom_bank;
}
