#include "sdl_pixel_display.h"
#include "SDL3/SDL_render.h"
#include "ppu.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool sdl_pixel_display_create(struct sdl_pixel_display *display,
                              const char *name, int px_width, int px_height,
                              int px_scale, SDL_WindowFlags flags) {
  display->window = NULL;
  display->renderer = NULL;
  display->screen = NULL;
  display->framebuffer = NULL;

  if (!SDL_CreateWindowAndRenderer(name, px_width * px_scale,
                                   px_height * px_scale, flags,
                                   &display->window, &display->renderer)) {
    fprintf(stderr, "sdl_pixel_display window & renderer: %s\n",
            SDL_GetError());
    goto sdl_pixel_display_cleanup;
  }

  if ((display->screen =
           SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA32,
                             SDL_TEXTUREACCESS_STREAMING, SCREEN_PIXL_W,
                             SCREEN_PIXL_H)) == NULL) {
    fprintf(stderr, "sdl_pixel_display screen texture: %s\n", SDL_GetError());
    goto sdl_pixel_display_cleanup;
  }

  if (!SDL_SetTextureScaleMode(display->screen, SDL_SCALEMODE_PIXELART)) {
    fprintf(stderr, "sdl_pixel_display screen scale mode: %s\n",
            SDL_GetError());
    goto sdl_pixel_display_cleanup;
  }

  if ((display->framebuffer = malloc(sizeof(SDL_Color) * display->screen->w *
                                     display->screen->h)) == NULL) {
    perror("sdl_pixel_display framebuffer malloc");
    goto sdl_pixel_display_cleanup;
  }

  return true;

sdl_pixel_display_cleanup:
  sdl_pixel_display_destroy(display);
  return false;
}

void sdl_pixel_display_destroy(struct sdl_pixel_display *display) {
  if (display->window) {
    SDL_DestroyWindow(display->window);
    display->window = NULL;
  }
  if (display->renderer) {
    SDL_DestroyRenderer(display->renderer);
    display->renderer = NULL;
  }
  if (display->screen) {
    SDL_DestroyTexture(display->screen);
    display->screen = NULL;
  }
  if (display->framebuffer) {
    free(display->framebuffer);
    display->framebuffer = NULL;
  }
}

static void display_garbage_test(struct sdl_pixel_display *display) {
  for (int y = 0; y < display->screen->h; ++y) {
    for (int x = 0; x < display->screen->w; ++x)
      display->framebuffer[x + y * display->screen->w] =
          (SDL_Color){.r = rand(), .g = rand(), .b = rand(), .a = 0xFF};
  }
}

void sdl_pixel_display_render_frame(struct sdl_pixel_display *display) {
  SDL_RenderClear(display->renderer);

  display_garbage_test(display);

  SDL_UpdateTexture(display->screen, NULL, display->framebuffer,
                    display->screen->w);

  SDL_RenderTexture(display->renderer, display->screen, NULL, NULL);
  SDL_RenderPresent(display->renderer);
}
