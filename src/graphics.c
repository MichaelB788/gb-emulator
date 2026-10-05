#include "graphics.h"
#include "SDL3/SDL_render.h"
#include "gameboy.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static constexpr uint8_t PIXL_SZ = 5;
static constexpr uint8_t SCREEN_PIXL_WIDTH = 160;
static constexpr uint8_t SCREEN_PIXL_HEIGHT = 144;

bool graphics_create(struct graphics *graphics) {
  if (!SDL_CreateWindowAndRenderer("GB", SCREEN_PIXL_WIDTH * PIXL_SZ,
                                   SCREEN_PIXL_HEIGHT * PIXL_SZ, 0,
                                   &graphics->window, &graphics->renderer)) {
    fprintf(stderr, "graphics window & renderer: %s", SDL_GetError());
    return false;
  }

  return true;
}

void graphics_destroy(struct graphics *graphics) {
  if (graphics->window) {
    SDL_DestroyWindow(graphics->window);
    graphics->window = nullptr;
  }
  if (graphics->renderer) {
    SDL_DestroyRenderer(graphics->renderer);
    graphics->renderer = nullptr;
  }
}

enum gb_clr {
  CLR_LIGHT_GREEN = 0,
  CLR_GREEN = 1,
  CLR_DARK_GREEN = 2,
  CLR_DARKER_GREEN = 3
};

static void graphics_set_color(struct graphics *graphics, enum gb_clr clr) {
  // Shades of green, from lightest to darkest
  static constexpr SDL_Color GB_PALETTE[4] = {{0xb9, 0xe0, 0x8d, 0xff},
                                              {0x8b, 0xa8, 0x6a, 0xff},
                                              {0x69, 0x7f, 0x50, 0xff},
                                              {0x3d, 0x49, 0x2e, 0xff}};
  const uint8_t i = clr & 0x3;
  SDL_SetRenderDrawColor(graphics->renderer, GB_PALETTE[i].r, GB_PALETTE[i].g,
                         GB_PALETTE[i].b, GB_PALETTE[i].a);
}

static void graphics_draw_pixel(struct graphics *graphics, uint8_t x, uint8_t y,
                                enum gb_clr clr) {
  graphics_set_color(graphics, clr);
  SDL_RenderFillRect(graphics->renderer,
                     &(SDL_FRect){PIXL_SZ * x, PIXL_SZ * y, PIXL_SZ, PIXL_SZ});
}

void graphics_clear(struct graphics *graphics) {
  graphics_set_color(graphics, CLR_LIGHT_GREEN);
  SDL_RenderClear(graphics->renderer);
}

void graphics_render_frame(struct graphics *graphics,
                           const struct gameboy *gb) {
  for (size_t y = 0; y < SCREEN_PIXL_HEIGHT; ++y) {
    for (size_t x = 0; x < SCREEN_PIXL_WIDTH; ++x)
      graphics_draw_pixel(graphics, x, y, x + y);
  }

  SDL_RenderPresent(graphics->renderer);
}
