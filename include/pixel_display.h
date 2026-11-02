#pragma once
#include <SDL3/SDL_video.h>
#include <stdbool.h>

typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;

struct pixel_display {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *screen;
  SDL_Color *framebuffer;
};

bool pixel_display_create(struct pixel_display *display, const char *name,
                          int px_width, int px_height, int px_scale,
                          SDL_WindowFlags flags);
void pixel_display_destroy(struct pixel_display *display);

void pixel_display_render_frame(struct pixel_display *display);
