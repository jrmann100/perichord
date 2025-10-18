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
	perichord.cpp \
	../src/main.cpp

C_SOURCES := shim/Audio/data_waveforms.c \
	shim/Audio/data_bandlimit_step.c

C_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(C_SOURCES:.c=.o)))
CXX_OBJECTS := $(addprefix $(BUILD_DIR)/,$(notdir $(SOURCES:.cpp=.o)))

INCLUDES := -I shim \
	-I ../include \
	-I ../lib/button_matrix/src \
	-I ../lib/debouncer/src \
	-I ../lib/harp/src \
	-I ../lib/potentiometer/src \
	-I ../lib/Hx_plateReverb \
	-I shim/Audio

DEFINES := -DKINETISL -DTEENSYDUINO=120

OPT_LEVEL := -O3
CFLAGS := $(OPT_LEVEL) -pthread $(INCLUDES) $(DEFINES) -MMD -MP
CXXFLAGS := $(OPT_LEVEL) -std=c++17 -pthread $(INCLUDES) $(DEFINES) -MMD -MP
EMCC_LINK_FLAGS := -s WASM=1 \
	-s MODULARIZE=1 \
	-s EXPORT_NAME='createPerichord' \
	-s EXPORTED_RUNTIME_METHODS=['addFunction','emscriptenRegisterAudioObject','emscriptenGetAudioObject','wasmMemory'] \
	-s ALLOW_MEMORY_GROWTH=1 \
	-s MAXIMUM_MEMORY=16MB \
	-s INITIAL_MEMORY=8MB \
	-s STACK_SIZE=64KB \
	-s NO_FILESYSTEM=1 \
	-s ENVIRONMENT=web \
	-s MALLOC=emmalloc \
	-s ASSERTIONS=0 \
	-s ALLOW_TABLE_GROWTH=1 \
	-s AUDIO_WORKLET=1 \
	-s WASM_WORKERS=1 \
	-pthread \
	-s ENVIRONMENT=web \
	-s EXPORT_ES6=1 \
	-s ERROR_ON_UNDEFINED_SYMBOLS=0 \
	--bind

.PHONY: all clean watch

all: $(OUTPUT)

# Link all object files together
$(OUTPUT): $(CXX_OBJECTS) $(C_OBJECTS) | $(DIST_DIR)
	@echo "Linking $@..."
	$(EMCC) $(EMCC_LINK_FLAGS) $^ -o $@

# Compile C sources
$(BUILD_DIR)/%.o: shim/Audio/%.c | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(EMCC) $(CFLAGS) -c $< -o $@

# Compile C++ sources from various directories
$(BUILD_DIR)/%.o: shim/Audio/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(EMCC) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: shim/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(EMCC) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(EMCC) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ../lib/Hx_plateReverb/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(EMCC) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: ../src/%.cpp | $(BUILD_DIR)
	@echo "Compiling $<..."
	$(EMCC) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR):
	mkdir -p $@

$(DIST_DIR):
	mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(DIST_DIR)

# Watch mode for auto-rebuild on file changes
# Usage: make watch
watch:
	bunx chokidar-cli "../**/*.cpp" "../**/*.c" "../**/*.h" -c "make"

dev:
	bunx five-server

# Include auto-generated dependency files
-include $(CXX_OBJECTS:.o=.d) $(C_OBJECTS:.o=.d)
