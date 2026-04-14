add_library(GB_Core
  bus.c
  cpu.c
  gameboy.c
  instructions.c
)
add_library(GB::Core ALIAS GB_Core)
