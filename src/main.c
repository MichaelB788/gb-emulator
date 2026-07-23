#include "gb_app.h"
#include <string.h>

int main(int argc, char **argv) {
  const bool logging_enabled =
      argv[2] && strncmp(argv[2], "--logging_enabled", 17) == 0;

  struct gb_app app = {0};
  app_init(&app, argv[1], logging_enabled);
  app_run(&app);
  app_close(&app);
}
