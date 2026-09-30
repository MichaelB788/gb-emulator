#pragma once
#include "lcd.h"
#include <stdint.h>

struct ppu {
  struct lcd lcd;

  uint8_t vram[8 * 1024]; // 8 KiB video ram, [0x8000, 0x9FFF]
  uint8_t oam[0x100];     // object attribute memory, [0xFE00, 0xFE9F]
};

void ppu_init(struct ppu *ppu);

[[nodiscard]] uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16);
[[nodiscard]] uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16);

void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8);
void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8);
