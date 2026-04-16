add_library(GB_Core
  bus.c
  cartridge.c
  cpu.c
  gameboy.c
  instructions.c
  rom.c
)
add_library(GB::Core ALIAS GB_Core)
