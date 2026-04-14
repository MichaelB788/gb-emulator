add_library(GB_Core
  bus.c
  cpu.c
)
add_library(GB::Core ALIAS GB_Core)
