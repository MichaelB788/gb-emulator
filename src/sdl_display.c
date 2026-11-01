#include "sdl_display.h"
#include "SDL3/SDL_render.h"
#include "ppu.h"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_video.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

bool sdl_display_create(struct sdl_display *display, const char *name,
                        int px_width, int px_height) {
  display->pixel_height = px_height;
  display->pixel_width = px_width;

  display->window = NULL;
  display->renderer = NULL;
  display->pixel_screen = NULL;
  display->framebuffer = NULL;

  if (!SDL_CreateWindowAndRenderer(name, px_width * PIXEL_SCALE,
                                   px_height * PIXEL_SCALE, 0, &display->window,
                                   &display->renderer)) {
    fprintf(stderr, "display window & renderer: %s", SDL_GetError());
    goto sdl_display_cleanup;
  }

  if ((display->pixel_screen =
           SDL_CreateTexture(display->renderer, SDL_PIXELFORMAT_RGBA32,
                             SDL_TEXTUREACCESS_STREAMING, SCREEN_PIXL_W,
                             SCREEN_PIXL_H)) == NULL) {
    fprintf(stderr, "display screen texture: %s", SDL_GetError());
    goto sdl_display_cleanup;
  }

  if (!SDL_SetTextureScaleMode(display->pixel_screen, SDL_SCALEMODE_PIXELART)) {
    fprintf(stderr, "display screen scale mode: %s", SDL_GetError());
    goto sdl_display_cleanup;
  }

  if ((display->framebuffer = malloc(sizeof(SDL_Color) * display->pixel_width *
                                     display->pixel_height)) == NULL) {
    perror("sdl_display framebuffer malloc");
    goto sdl_display_cleanup;
  }

  return true;

sdl_display_cleanup:
  sdl_display_destroy(display);
  return false;
}

void sdl_display_destroy(struct sdl_display *display) {
  display->pixel_width = display->pixel_height = 0;

  if (display->window) {
    SDL_DestroyWindow(display->window);
    display->window = NULL;
  }
  if (display->renderer) {
    SDL_DestroyRenderer(display->renderer);
    display->renderer = NULL;
  }
  if (display->pixel_screen) {
    SDL_DestroyTexture(display->pixel_screen);
    display->pixel_screen = NULL;
  }
  if (display->framebuffer) {
    free(display->framebuffer);
    display->framebuffer = NULL;
  }
}

static void display_garbage_test(struct sdl_display *display) {
  for (int y = 0; y < display->pixel_height; ++y) {
    for (int x = 0; x < display->pixel_width; ++x)
      display->framebuffer[x + y * display->pixel_width] =
          (SDL_Color){.r = rand(), .g = rand(), .b = rand(), .a = 0xFF};
  }
}

void sdl_display_render_frame(struct sdl_display *display) {
  SDL_RenderClear(display->renderer);

  display_garbage_test(display);

  SDL_UpdateTexture(display->pixel_screen, NULL, display->framebuffer,
                    display->pixel_width);

  SDL_RenderTexture(display->renderer, display->pixel_screen, NULL, NULL);
  SDL_RenderPresent(display->renderer);
}
