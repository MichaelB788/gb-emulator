set(EXECUTABLE_NAME ${CMAKE_PROJECT_NAME})
set(LIBRARY_NAME ${CMAKE_PROJECT_NAME}Source)

add_executable(${EXECUTABLE_NAME} main.c)
target_compile_options(${EXECUTABLE_NAME} PRIVATE
  $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_library(${LIBRARY_NAME}
  app.c
  bus.c
  cartridge.c
  cpu.c
  cpu_dbg.c
  cpu_instructions.c
  gameboy.c
  joypad.c
  lcd.c
  mbc1.c
  oam.c
  ppu.c
  sdl_pixel_display.c
  serial_transfer.c
  timer.c
  u8_array.c
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
  ${LIBRARY_NAME}
  SDL3::SDL3
)
