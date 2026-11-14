#pragma once
#include "lcd.h"
#include "oam.h"
#include <SDL3/SDL_pixels.h>
#include <stdint.h>

#define SCREEN_PIXEL_W (20u * 8u)
#define SCREEN_PIXEL_H (18u * 8u)

#define BYTES_PER_TILE 16u

#define VRAM_DATA_SCREEN_PIXEL_W (16u * 8u)
#define VRAM_DATA_SCREEN_PIXEL_H (24u * 8u)

struct bus;

struct ppu {
  struct lcd lcd;
  struct oam oam;
  uint8_t vram[0x2000]; // video RAM, [0x8000, 0x9FFF]
};

void ppu_tick(struct ppu *ppu, struct bus *bus);

uint8_t ppu_read_vram(const struct ppu *ppu, uint16_t a16);
void ppu_write_vram(struct ppu *ppu, uint16_t a16, uint8_t u8);

uint8_t ppu_read_oam(const struct ppu *ppu, uint16_t a16);
void ppu_write_oam(struct ppu *ppu, uint16_t a16, uint8_t u8);

void ppu_draw_vram_tile_data(const struct ppu *ppu, SDL_Color *framebuffer);
