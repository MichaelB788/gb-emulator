# dmgb

A GameBoy emulator written in C, using the SDL3 library for rendering and input
handling.

This emulator targets support for the original DMG model, hence the name.

## Prerequisites

This emulator uses platform-agnostic libraries and functions, so it should work
on Linux, Windows, and MacOS.

The minimum requirements are:

- [vcpkg](https://vcpkg.io/en/)
- Any C compiler
  - Linux: [gcc](https://gcc.gnu.org/install/)
  - Windows: [MSVC](https://learn.microsoft.com/en-us/cpp/overview/acquire-msvc?view=msvc-170)
  - MacOS: [clang](https://clang.llvm.org/)
- [CMake (ver. 3.15+)](https://cmake.org/download/)
- [Ninja](https://ninja-build.org/)

## Building

### Release

If you just want to try out the emulator, configure the project in `Release`
mode:

```bash
cmake --preset Release
```

Then, you can build or rebuild the project using:

```bash
cmake --build build
```

The emulator binary can be found under `build/Release/`

### Debug

For emulator development or testing, configure the project in `Debug` mode:

```bash
cmake --preset Debug
```

Then, you can build or rebuild the project using:

```bash
cmake --build build
```

The emulator binary can be found under `build/Debug/`
