set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

add_executable(${CMAKE_PROJECT_NAME} main.c)
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
  PROJECT_ROOT="${CMAKE_SOURCE_DIR}")

add_library(GB_Core
  gb_app.c
  bus.c
  byte_vector.c
  cartridge.c
  cpu.c
  cpu_instrs.c
  gb_emulator.c
  impl_cpu_instrs.c
  main.c
  mapper.c
  mbc1.c
  timer.c
)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE
  GB_Core
  SDL3::SDL3
)
