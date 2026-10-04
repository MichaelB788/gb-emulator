#include "graphics.h"
#include "SDL3/SDL_render.h"
#include "gameboy.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_video.h>
#include <stddef.h>
#include <stdio.h>

// Shades of green, from lightest to darkest
static constexpr SDL_Color palette[4] = {{0xb9, 0xe0, 0x8d, 0xff},
                                         {0x8b, 0xa8, 0x6a, 0xff},
                                         {0x69, 0x7f, 0x50, 0xff},
                                         {0x3d, 0x49, 0x2e, 0xff}};

bool graphics_create(struct graphics *graphics) {
  if (!SDL_CreateWindowAndRenderer("GB", 800, 800, 0, &graphics->window,
                                   &graphics->renderer)) {
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

void graphics_clear(struct graphics *graphics) {
  SDL_SetRenderDrawColor(graphics->renderer, palette[0].r, palette[0].g,
                         palette[0].b, palette[0].a);
  SDL_RenderClear(graphics->renderer);
}

void graphics_render_frame(struct graphics *graphics,
                           const struct gameboy *gb) {
  for (size_t i = 0; i < 4; ++i) {
    SDL_SetRenderDrawColor(graphics->renderer, palette[i].r, palette[i].g,
                           palette[i].b, palette[i].a);
    SDL_RenderFillRect(graphics->renderer, &(SDL_FRect){20 * i, 0, 20, 20});
  }

  SDL_RenderPresent(graphics->renderer);
}
