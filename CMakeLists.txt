cmake_minimum_required(VERSION 3.15)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED YES)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

project(
  GB_Emulator
  LANGUAGES C
)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}/include {CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

find_package(SDL3 CONFIG)
if (NOT ${SDL3_FOUND})
  message("Attempting to use vendored SDL3")
  add_subdirectory(extern/SDL EXCLUDE_FROM_ALL)
endif()

add_subdirectory(src)
