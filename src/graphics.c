#include "graphics.h"
#include "SDL3/SDL_render.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <stdint.h>
#include <stdio.h>

#define PIXL_SZ 5u
#define SCREEN_PIXL_W 160u
#define SCREEN_PIXL_H 144u
#define SCREEN_RES_W (SCREEN_PIXL_W * PIXL_SZ)
#define SCREEN_RES_H (SCREEN_PIXL_H * PIXL_SZ)

int graphics_create(struct graphics *graphics) {
  if (!SDL_CreateWindowAndRenderer("GameBoy", SCREEN_RES_W, SCREEN_RES_H, 0,
                                   &graphics->window, &graphics->renderer)) {
    fprintf(stderr, "graphics window & renderer: %s", SDL_GetError());
    goto graphics_fail;
  }

  if ((graphics->screen =
           SDL_CreateTexture(graphics->renderer, SDL_PIXELFORMAT_RGBA32,
                             SDL_TEXTUREACCESS_STREAMING, SCREEN_PIXL_W,
                             SCREEN_PIXL_H)) == NULL) {
    fprintf(stderr, "graphics screen texture: %s", SDL_GetError());
    goto graphics_fail;
  }

  if (!SDL_SetTextureScaleMode(graphics->screen, SDL_SCALEMODE_PIXELART)) {
    fprintf(stderr, "graphics screen scale mode: %s", SDL_GetError());
    goto graphics_fail;
  }

  return 0;

graphics_fail:
  graphics_destroy(graphics);
  return -1;
}

void graphics_destroy(struct graphics *graphics) {
  if (graphics->window) {
    SDL_DestroyWindow(graphics->window);
    graphics->window = NULL;
  }
  if (graphics->renderer) {
    SDL_DestroyRenderer(graphics->renderer);
    graphics->renderer = NULL;
  }
}

void graphics_render_frame(const struct graphics *graphics) {
  static const SDL_Color GB_PALETTE[4] = {
      [0] = {.r = 0xb9, .g = 0xe0, .b = 0x8d, .a = 0xff}, // Light green
      [1] = {.r = 0x8b, .g = 0xa8, .b = 0x6a, .a = 0xff}, // Green
      [2] = {.r = 0x69, .g = 0x7f, .b = 0x50, .a = 0xff}, // Dark green
      [3] = {.r = 0x3d, .g = 0x49, .b = 0x2e, .a = 0xff}  // Darker green
  };

  SDL_RenderClear(graphics->renderer);

  void *pixels;
  int pitch;
  if (SDL_LockTexture(graphics->screen, NULL, &pixels, &pitch)) {
    for (size_t y = 0; y < SCREEN_PIXL_H; ++y) {
      SDL_Color *row = pixels + y * pitch;
      for (size_t x = 0; x < SCREEN_PIXL_W; ++x) {
        // Test screen, renders a cool design
        row[x] = GB_PALETTE[x + y & 3];
      }
    }
    SDL_UnlockTexture(graphics->screen);
  } else {
    fprintf(stderr, "Could not lock texture: %s\n", SDL_GetError());
  }

  SDL_RenderTexture(graphics->renderer, graphics->screen, NULL, NULL);
  SDL_RenderPresent(graphics->renderer);
}
