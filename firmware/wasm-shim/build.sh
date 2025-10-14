#!/bin/bash
set -e

if ! command -v emcc &> /dev/null; then
    echo "Emscripten not found"
    exit 1
fi

mkdir -p dist

SOURCES="
shim/Arduino.cpp
../lib/Hx_plateReverb/effect_platervbstereo.cpp
../src/main.cpp
"

    # -I ../lib/AT42QT/src \
emcc -O2 -std=c++17 \
    -I shim \
    -I ../include \
    -I ../lib/button_matrix/src \
    -I ../lib/debouncer/src \
    -I ../lib/harp/src \
    -I ../lib/potentiometer/src \
    -I ../lib/Hx_plateReverb \
    -I shim/Audio -DKINETISL -DTEENSYDUINO=120 \
    -s WASM=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME='MinichordModule' \
    -s EXPORTED_FUNCTIONS='["_malloc","_free","_wasm_init","_wasm_setup","_wasm_loop","_wasm_trigger_chord_voice","_wasm_release_chord_voice","_wasm_trigger_all_chords","_wasm_set_parameter","_wasm_get_parameter","_wasm_load_preset","_wasm_save_preset","_wasm_midi_message","_midi_message_received","_get_shared_state","_init_timing","_interval_timer_callback"]' \
    -s EXPORTED_RUNTIME_METHODS='["ccall","cwrap","getValue","setValue","UTF8ToString","stringToUTF8","HEAPU8"]' \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=64MB \
    -s MAXIMUM_MEMORY=512MB \
    -s STACK_SIZE=16MB \
    -s NO_FILESYSTEM=1 \
    -s ENVIRONMENT=web \
    -s MALLOC=emmalloc \
    -s ASSERTIONS=0 \
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
    --bind \
    $SOURCES \
    -o dist/minichord.js

ls -lh dist/minichord.wasm dist/minichord.js