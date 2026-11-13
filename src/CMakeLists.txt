# ensure dependencies are installed
find_package(SDL3 CONFIG REQUIRED)

# lib
set(SOURCE_LIB ${CMAKE_PROJECT_NAME}Source)

add_library(${SOURCE_LIB}
  app.c
  bus.c
  cartridge.c
  cpu.c
  cpu_instructions.c
  gameboy.c
  joypad.c
  lcd.c
  mbc1.c
  oam.c
  pixel_display.c
  ppu.c
  serial_transfer.c
  timer.c
  u8_array.c
)

target_include_directories(${SOURCE_LIB} PUBLIC
  ${CMAKE_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_BINARY_DIR}/include
)

target_link_libraries(${SOURCE_LIB} PUBLIC
  SDL3::SDL3
)

# exe
set(EXE ${CMAKE_PROJECT_NAME})

add_executable(${EXE} main.c)

target_compile_options(${EXE} PRIVATE
  $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

target_link_libraries(${EXE} PRIVATE
  ${SOURCE_LIB}
)
