cmake_minimum_required(VERSION 4.3.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)

project(
  GameBoy
  VERSION 1.0
  LANGUAGES C
)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/include {CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

add_subdirectory(src)
