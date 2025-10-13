#!/bin/bash

# Build script for MiniChord WASM
# Requires Emscripten SDK: https://emscripten.org/docs/getting_started/downloads.html

set -e

echo "Building MiniChord WASM shim layer..."

# Paths
SHIM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
FIRMWARE_DIR="$(dirname "$SHIM_DIR")"
SRC_DIR="$FIRMWARE_DIR/src"
INCLUDE_DIR="$FIRMWARE_DIR/include"
OUTPUT_DIR="$SHIM_DIR/build"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Compiler settings
CC=emcc
CFLAGS="-O2"
CFLAGS="$CFLAGS -I$SHIM_DIR"
CFLAGS="$CFLAGS -I$INCLUDE_DIR"
CFLAGS="$CFLAGS -I$FIRMWARE_DIR/lib"
CFLAGS="$CFLAGS -s WASM=1"
CFLAGS="$CFLAGS -s ALLOW_MEMORY_GROWTH=1"
CFLAGS="$CFLAGS -s EXPORTED_FUNCTIONS=[_malloc,_free]"
CFLAGS="$CFLAGS -s EXPORTED_RUNTIME_METHODS=[ccall,cwrap,UTF8ToString,stringToUTF8]"
CFLAGS="$CFLAGS -s MODULARIZE=1"
CFLAGS="$CFLAGS -s EXPORT_NAME=MinichordModule"
CFLAGS="$CFLAGS --js-library $SHIM_DIR/library.js"

# Source files
SOURCES=(
    "$SHIM_DIR/Arduino.cpp"
    "$SHIM_DIR/Audio.cpp"
    # Add more source files as needed
    # "$SRC_DIR/main.cpp"  # Uncomment when ready to compile full firmware
)

echo "Compiling sources..."
$CC $CFLAGS "${SOURCES[@]}" -o "$OUTPUT_DIR/minichord.js"

echo "Build complete!"
echo "Output: $OUTPUT_DIR/minichord.js"
echo "        $OUTPUT_DIR/minichord.wasm"
echo ""
echo "To test, run: cd $SHIM_DIR && python3 -m http.server 8000"
echo "Then open: http://localhost:8000/test.html"
