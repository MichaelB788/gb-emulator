#pragma once
#include <stdbool.h>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Color SDL_Color;

#define PIXEL_SCALE 5ul

struct sdl_pixel_display {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *screen;
  SDL_Color *framebuffer;
};

bool sdl_pixel_display_create(struct sdl_pixel_display *display,
                              const char *name, int px_width, int px_height);
void sdl_pixel_display_destroy(struct sdl_pixel_display *display);

void sdl_pixel_display_render_frame(struct sdl_pixel_display *display);
