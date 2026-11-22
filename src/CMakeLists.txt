# ensure sdl3 library is installed
find_package(SDL3 CONFIG REQUIRED)

# lib
set(SOURCE_LIB ${CMAKE_PROJECT_NAME}Source)
add_library(${SOURCE_LIB}
  dmgb/ppu/lcd.c
  dmgb/ppu/oam.c
  dmgb/ppu/ppu.c

  dmgb/cartridge/cartridge.c
  dmgb/cartridge/mbc1.c

  dmgb/dmgb.c
  dmgb/bus.c
  dmgb/instructions.c
  dmgb/joypad.c
  dmgb/serial_transfer.c
  dmgb/register_file.c
  dmgb/timer.c

  app.c
  pixel_display.c
)
target_link_libraries(${SOURCE_LIB} PUBLIC SDL3::SDL3)
target_include_directories(${SOURCE_LIB} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# exe
set(EXE ${CMAKE_PROJECT_NAME})
add_executable(${EXE} main.c)
target_link_libraries(${EXE} PRIVATE ${SOURCE_LIB})
target_compile_options(${EXE} PRIVATE
  $<$<C_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<C_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
