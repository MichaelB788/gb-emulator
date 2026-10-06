#pragma once

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
typedef struct SDL_Texture SDL_Texture;
struct gameboy;

struct graphics {
  SDL_Window *window;
  SDL_Renderer *renderer;
  SDL_Texture *screen;
};

int graphics_create(struct graphics *graphics);
void graphics_destroy(struct graphics *graphics);

void graphics_render_frame(const struct graphics *graphics);
