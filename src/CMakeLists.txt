set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
add_executable(${CMAKE_PROJECT_NAME} main.c)

add_subdirectory(core)

target_link_libraries(${CMAKE_PROJECT_NAME}
  GB::Core
)

target_compile_options(${CMAKE_PROJECT_NAME} PRIVATE
  -Wall
  -Wextra
  -Wpedantic
  -Wconversion
  -Wsign-conversion
  -Wnarrowing
)
