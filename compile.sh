#!/usr/bin/env bash

set -euo pipefail

PROJECT_ROOT=$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)
BUILD_DIR="$PROJECT_ROOT/build"
BUILD_TYPE="${BUILD_TYPE:-Debug}"

cmake -S "$PROJECT_ROOT" -B "$BUILD_DIR" \
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
  -DCMAKE_BUILD_TYPE="$BUILD_TYPE" \
  -G Ninja

ninja -C "$BUILD_DIR"

echo "Build successful. Program can be run with:"
echo "./build/bin/GB_Emulator <path/to/rom>"
