# GameBoy Emulator

A GameBoy emulator written in C, using the SDL3 library for rendering and input
handling.

# Requirements

- vcpkg
- GCC (Linux), MSVC (Windows), or Clang (MacOS)
- cmake
- Ninja

# Build Instructions

To try out the project, build it in `Release` mode:

```bash
cmake --preset Release
```

To for develop/test the project, build it in `Debug` mode:

```bash
cmake --preset Debug
```

The project can then be (re)built by running

```bash
cmake --build build
```

The binary can be found under `build/Debug/` or `build/Release`, depending on
the build type selected.
