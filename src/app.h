#pragma once
#include "dmgb/dmgb.h"
#include "pixel_display.h"
#include <stdbool.h>
#include <stdint.h>

struct app {
  uint64_t prev_time, curr_time;
  struct dmgb dmgb;
  struct pixel_display main_display;
  struct pixel_display vram_tile_data_display;
};

bool app_create(struct app *app, const char *rom, int px_scale);
void app_destroy(struct app *app);

void app_update(struct app *app);
void app_render(struct app *app);
void app_sleep(const struct app *app);
