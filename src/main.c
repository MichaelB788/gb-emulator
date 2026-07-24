#include "appstate.h"
#include <string.h>

int main(int argc, char **argv) {
  const bool logging_enabled = argv[2] && strncmp(argv[2], "--log", 5) == 0;
  struct appstate app = {0};
  app_init(&app, argv[1], logging_enabled);
  app_run(&app);
  app_close(&app);
}
