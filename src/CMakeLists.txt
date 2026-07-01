set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(${CMAKE_PROJECT_NAME} main.c)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_library(GB_Core
  cartridge.c
  cpu.c
  gameboy.c
  instructions.c
  interrupt.c
  mbc1.c
  serial.c
  timer.c
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE GB_Core)
