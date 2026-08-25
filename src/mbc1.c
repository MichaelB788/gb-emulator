#include "mbc1.h"
#include "constants.h"
#include "u8_buf.h"
#include <stddef.h>
#include <stdint.h>

static void mbc1_set_rom_bank(struct mbc1 *mbc1, uint8_t val, size_t rom_cap) {
  // Masks off any unused bits when setting the ROM bank
  const uint8_t mask = (rom_cap / KiB_16) - 1;
  if (mask > 0x1F) {
    mbc1->rom_bank = mbc1->mode == BANKING_ADVANCED
                         ? (mbc1->ram_bank << 5) | val & 0x1F
                         : val & 0x1F;
  } else {
    mbc1->rom_bank = val & mask;
  }

  if ((mbc1->rom_bank & 0x1F) == 0)
    ++mbc1->rom_bank;
}

void mbc1_init(struct mbc1 *mbc) {
  mbc->rom_bank = 1;
  mbc->ram_bank = 0;
  mbc->mode = BANKING_SIMPLE;
  mbc->ram_enabled = false;
}

uint8_t mbc1_read_rom(const struct mbc1 *mbc1, const struct u8_buf *rom,
                      const uint16_t addr) {
  return addr < 0x4000 ? rom->data[addr]
                       : rom->data[(KiB_16 * mbc1->rom_bank) + (addr - 0x4000)];
}

uint8_t mbc1_read_ram(const struct mbc1 *mbc1, const struct u8_buf *ram,
                      const uint16_t addr) {
  return mbc1->ram_enabled
             ? ram->data[(addr - 0xA000) + (mbc1->ram_bank * KiB_8)]
             : 0xFF;
}

// See: https://gbdev.io/pandocs/MBC1.html#registers
void mbc1_write_rom(struct mbc1 *mbc1, const struct u8_buf *rom, uint16_t addr,
                    uint8_t val) {
  if (addr <= 0x1FFF)
    mbc1->ram_enabled = (val & 0xF) == 0xA;
  else if (0x2000 <= addr && addr <= 0x3FFF)
    mbc1_set_rom_bank(mbc1, val, rom->cap);
  else if (0x4000 <= addr && addr <= 0x5FFF)
    mbc1->ram_bank = val & 0x3;
  else if (0x6000 <= addr && addr <= 0x7FFF)
    mbc1->mode = (enum banking_mode)(val & 1);
}

void mbc1_write_ram(const struct mbc1 *mbc1, struct u8_buf *ram, uint16_t addr,
                    uint8_t val) {
  if (mbc1->ram_enabled)
    ram->data[(addr - 0xA000) + (mbc1->ram_bank * KiB_8)] = val;
}
