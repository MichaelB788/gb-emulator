#pragma once
#include <stdbool.h>

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
typedef struct SDL_Color SDL_Color;

#define PIXEL_SCALE 5ul

struct sdl_display {
  int pixel_width, pixel_height;

  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *pixel_screen;

  SDL_Color *framebuffer;
};

bool sdl_display_create(struct sdl_display *display, const char *name,
                        int px_width, int px_height);
void sdl_display_destroy(struct sdl_display *display);

void sdl_display_render_frame(struct sdl_display *display);
