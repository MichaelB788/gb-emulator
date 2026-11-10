#pragma once

struct app;

struct app *app_create(const char *rom, int px_scale);
void app_destroy(struct app *state);

void app_update(struct app *app);
void app_render(struct app *app);
void app_sleep(struct app *app);
