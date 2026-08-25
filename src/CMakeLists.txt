set(EXECUTABLE_NAME ${CMAKE_PROJECT_NAME})
set(LIBRARY_NAME ${CMAKE_PROJECT_NAME}Source)

add_executable(${EXECUTABLE_NAME} main.c)
target_compile_options(${EXECUTABLE_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)

add_library(${LIBRARY_NAME}
  appstate.c
  bus.c
  cartridge.c
  cpu.c
  cpu_instrs.c
  debugger.c
  gameboy.c
  impl_cpu_instrs.c
  interrupts.c
  joypad.c
  main.c
  mbc1.c
  serial.c
  timer.c
  u8_buf.c
)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE
  ${LIBRARY_NAME}
  SDL3::SDL3
)
