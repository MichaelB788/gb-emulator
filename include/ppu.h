#pragma once
#include "lcd.h"
#include "oam.h"
#include <SDL3/SDL_pixels.h>
#include <stdint.h>

#define SCREEN_PIXL_W 160ul
#define SCREEN_PIXL_H 144ul

#define VRAM_TILE_DATA_TILE_W 16ul
#define VRAM_TILE_DATA_TILE_H 24ul
#define VRAM_TILE_DATA_PIXEL_W (VRAM_TILE_DATA_TILE_W * 8ul)
#define VRAM_TILE_DATA_PIXEL_H (VRAM_TILE_DATA_TILE_H * 8ul)

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
