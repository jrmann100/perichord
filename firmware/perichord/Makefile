# WebAssembly build using Emscripten for the browser shim
EMCC := emcc
EMCC_PATH := $(shell command -v $(EMCC) 2>/dev/null)

ifeq ($(strip $(EMCC_PATH)),)
$(error Emscripten not found)
endif

DIST_DIR := dist
BUILD_DIR := build
OUTPUT := $(DIST_DIR)/perichord.js

AUDIO_SOURCES := $(wildcard shim/Audio/*.cpp)
SOURCES := $(AUDIO_SOURCES) \
    shim/AudioStream.cpp \
    shim/Arduino.cpp \
    shim/button_matrix.cpp \
    shim/debouncer.cpp \
    shim/harp.cpp \
    shim/MPR121.cpp \
    shim/potentiometer.cpp \
    ../lib/Hx_plateReverb/effect_platervbstereo.cpp \
    perichord.cpp

C_SOURCES := shim/Audio/data_waveforms.c \
    shim/Audio/data_bandlimit_step.c

C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))

INCLUDES := -I shim \
    -I ../include \
    -I ../lib/button_matrix/src \
    -I ../lib/debouncer/src \
    -I ../lib/harp/src \
    -I ../lib/potentiometer/src \
    -I ../lib/Hx_plateReverb \
    -I shim/Audio

DEFINES := -DKINETISL -DTEENSYDUINO=120

CFLAGS := -O2 -pthread $(INCLUDES) $(DEFINES)
CXXFLAGS := -O2 -std=c++17 -pthread $(INCLUDES) $(DEFINES)
EMCC_LINK_FLAGS := -s WASM=1 \
    -s MODULARIZE=1 \
    -s EXPORT_NAME='createPerichord' \
    -s EXPORTED_RUNTIME_METHODS=['addFunction','emscriptenRegisterAudioObject','emscriptenGetAudioObject'] \
    -s ALLOW_MEMORY_GROWTH=1 \
    -s INITIAL_MEMORY=8MB \
    -s MAXIMUM_MEMORY=16MB \
    -s STACK_SIZE=64KB \
    -s NO_FILESYSTEM=1 \
    -s ENVIRONMENT=web \
    -s MALLOC=emmalloc \
    -s ASSERTIONS=0 \
    -s ALLOW_TABLE_GROWTH=1 \
    -s AUDIO_WORKLET=2 \
    -s WASM_WORKERS=1 \
    -pthread \
    -s ENVIRONMENT=web \
    -s EXPORT_ES6=1 \
    -s ERROR_ON_UNDEFINED_SYMBOLS=0 \
    --bind

.PHONY: all clean

all: $(OUTPUT)

$(OUTPUT): $(SOURCES) $(C_OBJECTS) | $(DIST_DIR)
	$(EMCC) $(CXXFLAGS) $(EMCC_LINK_FLAGS) $(SOURCES) $(C_OBJECTS) -o $@

$(BUILD_DIR)/%.o: shim/Audio/%.c | $(BUILD_DIR)
	$(EMCC) $(CFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(DIST_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)
