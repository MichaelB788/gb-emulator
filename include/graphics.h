#pragma once

typedef struct SDL_Window SDL_Window;
typedef struct SDL_Renderer SDL_Renderer;
struct gameboy;

struct graphics {
  SDL_Window *window;
  SDL_Renderer *renderer;
};

[[nodiscard]] bool graphics_create(struct graphics *graphics);
void graphics_destroy(struct graphics *graphics);

void graphics_clear(struct graphics *graphics);

void graphics_render_frame(struct graphics *graphics, const struct gameboy *gb);
