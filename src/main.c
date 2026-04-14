#include "core/bus.h"
#include "core/cpu.h"
#include <stdio.h>

int main(void) {
  Bus bus;
  CPU cpu = {.bus = &bus};
  printf("%i", read(&bus, 0x10));
}
