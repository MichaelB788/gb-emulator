# GameBoy Emulator

A GameBoy emulator written in C, purely for the fun of it.

## Building on Linux

`cd` into the project root and compile the program using the following:

```bash
cmake -B build -S .\
    -DCMAKE_EXPORT_COMPILE_COMMANDS=YES\
    -DCMAKE_BUILD_TYPE=Debug\
    -G Ninja
```

The program can then be built by running

```bash
cmake --build build
```

The binary can be found under `build/Debug/`.
