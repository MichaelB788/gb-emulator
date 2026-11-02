set(TARGET_EXE ${CMAKE_PROJECT_NAME})
set(LIBRARY_NAME ${CMAKE_PROJECT_NAME}Source)

add_executable(${TARGET_EXE} main.c)

target_compile_options(${TARGET_EXE} PRIVATE
  $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_library(${LIBRARY_NAME}
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

target_link_libraries(${TARGET_EXE} PRIVATE
  ${LIBRARY_NAME}
  SDL3::SDL3
)
