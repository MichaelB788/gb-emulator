#include "mbc1.h"
#include <stddef.h>
#include <stdint.h>

uint8_t mbc1_rom_read(const struct mbc1 *mbc1, const struct u8_array *rom,
                      uint16_t a16) {
  return a16 < 0x4000 ? rom->data[a16]
                      : rom->data[(a16 - 0x4000) + (mbc1->rom_bank * 0x4000)];
}

static void mbc1_rom_bank_write(struct mbc1 *mbc1, uint8_t u8,
                                size_t rom_size) {
  const uint8_t mask = (rom_size / 0x4000) - 1;
  if (mask > 0x1F) {
    mbc1->rom_bank = mbc1->is_advanced_banking_enabled == 0
                         ? u8 & 0x1F
                         : mbc1->ram_bank << 5 | u8 & 0x1F;
  } else {
    mbc1->rom_bank = u8 & mask;
  }

  if ((mbc1->rom_bank & 0x1F) == 0)
    ++mbc1->rom_bank;
}

void mbc1_rom_write(struct mbc1 *mbc1, const struct u8_array *rom, uint16_t a16,
                    uint8_t u8) {
  if (a16 <= 0x1FFF)
    mbc1->is_ram_enabled = (u8 & 0xF) == 0xA;
  else if (0x2000 <= a16 && a16 <= 0x3FFF)
    mbc1_rom_bank_write(mbc1, u8, rom->size);
  else if (0x4000 <= a16 && a16 <= 0x5FFF)
    mbc1->ram_bank = u8 & 0x3;
  else if (0x6000 <= a16 && a16 <= 0x7FFF)
    mbc1->is_advanced_banking_enabled = u8 & 1;
}

uint8_t mbc1_read_ram(const struct mbc1 *mbc1, const struct u8_array *ram,
                      uint16_t a16) {
  return mbc1->is_ram_enabled
             ? ram->data[a16 - 0xA000 + mbc1->ram_bank * 0x2000]
             : 0xFF;
}

void mbc1_write_ram(const struct mbc1 *mbc1, struct u8_array *ram, uint16_t a16,
                    uint8_t u8) {
  if (mbc1->is_ram_enabled)
    ram->data[a16 - 0xA000 + mbc1->ram_bank * 0x2000] = u8;
}
