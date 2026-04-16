#pragma once
#include "core/rom.h"
#include <stdint.h>

typedef enum { ROM_ONLY } MBC_Type;

typedef struct {
  MBC_Type type;
  union {
    ROM rom;
    // TODO: Add more MBCs here
  };
} MBC;
