add_library(GB_Core
  bus.c
  cartridge.c
  cpu.c
  gameboy.c
  instructions.c
  io.c
  mbc1.c
  mapper.c
)
add_library(GB::Core ALIAS GB_Core)
