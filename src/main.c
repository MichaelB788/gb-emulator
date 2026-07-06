#include "gameboy.h"
#include "optables.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_main.h>

static struct gameboy gb = {0};
static FILE *log_file = NULL;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv) {
  if (argc < 2) {
    fprintf(stderr, "No arguments given, aborting.\n");
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_EVENTS)) {
    SDL_Log("Could not initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  if (!init_gameboy(&gb, argv[1])) {
    fprintf(stderr, "Could not initialize the gameboy.\n");
    return SDL_APP_FAILURE;
  }

  if (argv[2] && strcmp(argv[2], "--logging_enabled") == 0) {
    log_file = fopen("log.txt", "w");
    if (!log_file) {
      perror("Could not open log file");
    }
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {
  int cycles = 0;
  gb.cpu.IME = gb.cpu.enable_interrupts;

  gb.opcode = bus_read(&gb, gb.cpu.PC);
  if (log_file) {
    log_curr_instr(&gb, log_file);
  }
  ++gb.cpu.PC;

  cycles += unprefixed_ins[gb.opcode](&gb);
  cycles += handle_interrupts(&gb);

  timer_tick(&gb.timer, cycles, &gb.interrupt);

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  switch (event->type) {
  case SDL_EVENT_QUIT:
    return SDL_APP_SUCCESS;
  default:
    break;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  close_gameboy(&gb);

  if (log_file) {
    fclose(log_file);
  }

  if (result == SDL_APP_SUCCESS) {
    printf("Program exited successfully\n");
  } else {
    printf("Program exited unsuccessfully with: %d\n", result);
  }
}
