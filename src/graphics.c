#include "graphics.h"
#include "SDL3/SDL_render.h"
#include "lcd.h"
#include "ppu.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#define PIXL_SZ 5u
#define SCREEN_RES_W (LCD_SCREEN_PIXL_W * PIXL_SZ)
#define SCREEN_RES_H (LCD_SCREEN_PIXL_H * PIXL_SZ)

int graphics_create(struct graphics *graphics) {
  if (!SDL_CreateWindowAndRenderer("GameBoy", SCREEN_RES_W, SCREEN_RES_H, 0,
                                   &graphics->window, &graphics->renderer)) {
    fprintf(stderr, "graphics window & renderer: %s", SDL_GetError());
    goto graphics_fail;
  }

  if ((graphics->screen =
           SDL_CreateTexture(graphics->renderer, SDL_PIXELFORMAT_RGBA32,
                             SDL_TEXTUREACCESS_STREAMING, LCD_SCREEN_PIXL_W,
                             LCD_SCREEN_PIXL_H)) == NULL) {
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
  if (graphics->screen) {
    SDL_DestroyTexture(graphics->screen);
    graphics->screen = NULL;
  }
}

void graphics_render_frame(const struct graphics *graphics,
                           const struct ppu *ppu) {
  SDL_RenderClear(graphics->renderer);

  void *pixels;
  int pitch;
  if (SDL_LockTexture(graphics->screen, NULL, &pixels, &pitch)) {
    for (size_t y = 0; y < LCD_SCREEN_PIXL_H; ++y) {
      SDL_Color *row = pixels + y * pitch;
      for (size_t x = 0; x < LCD_SCREEN_PIXL_W; ++x) {
        row[x] = ppu->lcd.framebuffer[x + y * LCD_SCREEN_PIXL_W];
      }
    }
    SDL_UnlockTexture(graphics->screen);
  } else {
    fprintf(stderr, "Could not lock texture: %s\n", SDL_GetError());
  }

  SDL_RenderTexture(graphics->renderer, graphics->screen, NULL, NULL);
  SDL_RenderPresent(graphics->renderer);
}
