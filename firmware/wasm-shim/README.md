# WASM Shim Layer for MiniChord

This directory contains C/C++ shim implementations of Teensy/Arduino libraries that allow the MiniChord firmware to compile to WebAssembly and run in a browser using Web Audio API.

## Architecture

The shim layer replaces hardware-dependent libraries with browser-compatible implementations:

- **Arduino.h**: Timing functions, I/O stubs
- **Audio.h**: Teensy Audio Library → Web Audio API bridge
- **usbMIDI**: USB MIDI → Web MIDI API bridge  
- **LittleFS**: File system → IndexedDB/localStorage
- **Hardware libs**: Touch sensors, buttons → Virtual controls/MIDI mapping

## Compilation Strategy

1. Keep original firmware source files unchanged
2. Provide shim headers/implementations with same API signatures
3. Compile with Emscripten: `emcc -o minichord.wasm`
4. Bridge C++ objects to JavaScript via Embind
5. JavaScript layer manages actual Web Audio graph and UI

## Implementation Approach

### Two-Layer Architecture:

**Layer 1: C++ Shim (WASM)**
- Implements Audio class APIs
- Maintains audio graph structure
- Exposes parameter setters/getters
- Handles timing and control flow

**Layer 2: JavaScript Bridge**
- Creates actual Web Audio nodes
- Connects audio graph based on C++ state
- Handles Web MIDI input
- Manages UI interaction
- Renders to AudioContext

## Key Challenges

1. **Audio Graph Complexity**: 216 AudioConnection objects need translation
2. **Real-time Requirements**: AudioWorklet for low-latency processing
3. **Memory Model**: C++ objects ↔ JS Web Audio nodes synchronization
4. **State Management**: Keeping C++ and JS audio graphs in sync

## Development Phases

### Phase 1: Core Audio Infrastructure
- [ ] Basic AudioStream/AudioConnection framework
- [ ] Simple oscillator (AudioSynthWaveform)
- [ ] Basic envelope (AudioEffectEnvelope)
- [ ] Output routing (AudioOutputUSB → Web Audio destination)

### Phase 2: Essential Synth Components  
- [ ] AudioSynthWaveformModulated (FM synthesis)
- [ ] AudioFilterStateVariable (filters)
- [ ] AudioMixer4 (mixing)
- [ ] AudioEffectEnvelope (full ADSR)

### Phase 3: Effects & Polish
- [ ] AudioEffectDelay
- [ ] AudioEffectWaveshaper
- [ ] Plate reverb
- [ ] All remaining audio effects

### Phase 4: Control & I/O
- [ ] Parameter system (sysex)
- [ ] Web MIDI integration
- [ ] Virtual control surface
- [ ] Preset storage (IndexedDB)

## Files

- `Arduino.h/cpp` - Basic Arduino API shims
- `Audio.h/cpp` - Teensy Audio Library shims
- `AudioStream.h/cpp` - Base audio processing class
- `usbMIDI.h/cpp` - MIDI interface shim
- `embind_bridge.cpp` - Embind bindings for JS↔C++ communication
- `js/audio-bridge.js` - JavaScript Web Audio implementation
- `build.sh` - Emscripten build script

## Building

```bash
cd wasm-shim
./build.sh
```

This will produce `minichord.wasm` and `minichord.js` for use in a web page.

## Testing

```bash
npm install
npm run dev  # Starts local server with test harness
```

## Resources

- [Emscripten Documentation](https://emscripten.org/)
- [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)
- [Teensy Audio Library](https://github.com/PaulStoffregen/Audio)
- [AudioWorklet](https://developer.mozilla.org/en-US/docs/Web/API/AudioWorklet)
