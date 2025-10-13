# MiniChord WASM Port - Implementation Guide

## What You've Got Here

This is a **shim layer approach** to porting the MiniChord Teensy firmware to WebAssembly. Instead of rewriting the firmware, you replace the hardware-dependent libraries (Arduino, Teensy Audio) with browser-compatible implementations that have the **exact same API**.

## Architecture Overview

```
Original Firmware (C++)          Your Shim Layer (C++)           Browser (JavaScript)
━━━━━━━━━━━━━━━━━━━━━            ━━━━━━━━━━━━━━━━━━━━            ━━━━━━━━━━━━━━━━━━━
main.cpp                    ┌─→  Arduino.h/cpp          ┌─→  performance.now()
  #include <Audio.h> ───────┤    - millis(), micros()   │    - console.log()
  #include <Arduino.h> ─────┘    - Serial class         │    
                                  - pinMode(), etc.      │
audio_definition.h          ┌─→  Audio.h/cpp            └─→  Web Audio API
  AudioSynthWaveform ───────┤    - AudioStream base          - OscillatorNode
  AudioEffectEnvelope ──────┤    - All audio classes         - BiquadFilterNode
  AudioConnection ──────────┘    - Same API, different       - GainNode
                                   implementation             - DelayNode
                                                              - etc.
                                        │
                                        ├─ Compiled to WASM via Emscripten
                                        └─ JavaScript bridge (audio-bridge.js)
```

## Key Files Created

### 1. **Arduino.h/cpp** - Basic MCU Abstraction
- Timing: `millis()`, `micros()`, `delay()` → JavaScript `performance.now()`
- I/O: `pinMode()`, `digitalWrite()`, `analogRead()` → Virtual/no-op
- Serial: `Serial.print()` → `console.log()`
- Timers: `IntervalTimer`, `elapsedMillis` → JavaScript timers

### 2. **Audio.h/cpp** - Audio Library Shim
Implements all the Teensy Audio classes:
- **Oscillators**: `AudioSynthWaveform`, `AudioSynthWaveformModulated`, `AudioSynthWaveformDc`, `AudioSynthNoiseWhite`
- **Effects**: `AudioEffectEnvelope`, `AudioFilterStateVariable`, `AudioEffectDelay`, `AudioEffectWaveshaper`, `AudioEffectMultiply`
- **Mixing**: `AudioMixer4`, `AudioAmplifier`
- **I/O**: `AudioOutputUSB`, `AudioOutputI2S`, `AudioInputI2S`
- **Infrastructure**: `AudioStream`, `AudioConnection`, `AudioMemory()`

Each C++ object has a unique ID and notifies JavaScript to create/configure corresponding Web Audio nodes.

### 3. **audio-bridge.js** - JavaScript Web Audio Implementation
- Maps C++ object IDs to Web Audio API nodes
- Implements actual audio processing in the browser
- Handles parameter changes and audio routing
- Manages the Web Audio context

### 4. **library.js** - Emscripten JS Library
Defines the C→JavaScript bridge functions that WASM calls

### 5. **test.html** - Demo Page
Interactive test interface with:
- Virtual keyboard (12 notes)
- Parameter controls
- Test oscillators and envelopes
- Console output

## How It Works

1. **Firmware calls C++ Audio API**
   ```cpp
   AudioSynthWaveform osc;
   AudioEffectEnvelope env;
   AudioOutputUSB output;
   
   AudioConnection c1(osc, env);
   AudioConnection c2(env, output);
   
   osc.frequency(440);
   env.noteOn();
   ```

2. **C++ shim notifies JavaScript**
   ```cpp
   void AudioSynthWaveform::frequency(float freq) {
       frequency_ = freq;
       js_audio_set_param(object_id, "frequency", freq);
   }
   ```

3. **JavaScript creates Web Audio nodes**
   ```javascript
   setParam(objectId, 'frequency', value) {
       const obj = this.audioObjects.get(objectId);
       obj.oscillator.frequency.setValueAtTime(value, 
           this.audioContext.currentTime);
   }
   ```

## What's Missing (TODOs)

### Must-Have for Basic Functionality:
- [ ] `Wire.h`, `SPI.h` shims (can be no-ops)
- [ ] `usbMIDI` class → Web MIDI API
- [ ] `LittleFS` → IndexedDB for preset storage
- [ ] Hardware sensor classes → Virtual controls
- [ ] Plate reverb effect implementation
- [ ] Complete all AudioWorklet for complex effects

### Nice-to-Have:
- [ ] AudioWorklet for better performance
- [ ] Web MIDI controller mapping
- [ ] Virtual control surface UI
- [ ] Preset browser/editor
- [ ] Visual audio graph display
- [ ] Performance optimizations

## Building

### Prerequisites
1. Install Emscripten SDK:
   ```bash
   git clone https://github.com/emscripten-core/emsdk.git
   cd emsdk
   ./emsdk install latest
   ./emsdk activate latest
   source ./emsdk_env.sh
   ```

2. Make build script executable:
   ```bash
   chmod +x build.sh
   ```

### Compile
```bash
./build.sh
```

This produces:
- `build/minichord.js` - JavaScript glue code
- `build/minichord.wasm` - Compiled C++ code

### Test
```bash
python3 -m http.server 8000
```
Then open: http://localhost:8000/test.html

## Next Steps

### Phase 1: Prove the Concept ✅
- [x] Basic Arduino shims
- [x] Core Audio classes
- [x] JavaScript bridge
- [x] Test harness
- [x] Simple oscillator + envelope working

### Phase 2: Audio Subsystem
- [ ] Implement all used Audio classes from firmware
- [ ] Add AudioWorklet for better performance
- [ ] Test complex audio graphs (12 strings + 4 voices)
- [ ] Implement plate reverb

### Phase 3: Control Interface
- [ ] Virtual button matrix
- [ ] Virtual potentiometers (Web MIDI CC)
- [ ] Web MIDI input handling
- [ ] Parameter system (sysex emulation)

### Phase 4: Integration
- [ ] Compile full `main.cpp` (may need tweaks)
- [ ] Hook up all controls
- [ ] Preset save/load (IndexedDB)
- [ ] UI polish

### Phase 5: Polish
- [ ] Performance optimization
- [ ] Mobile support
- [ ] PWA (installable web app)
- [ ] Documentation

## Advantages of This Approach

1. **Original code stays untouched** - No firmware rewrite
2. **Incremental development** - Build piece by piece
3. **Same logic** - Musical behavior is identical
4. **Debuggable** - Can test in browser dev tools
5. **Portable** - Runs on any device with a browser

## Challenges

1. **Real-time audio is hard** - Web Audio has latency
2. **Complex graph** - 216+ audio connections to manage
3. **Effects** - Some need custom AudioWorklet
4. **Memory model** - C++ ↔ JS synchronization
5. **Hardware inputs** - Need virtual replacements

## Resources

- [Emscripten Docs](https://emscripten.org/docs/)
- [Web Audio API](https://developer.mozilla.org/en-US/docs/Web/API/Web_Audio_API)
- [AudioWorklet Guide](https://developers.google.com/web/updates/2017/12/audio-worklet)
- [Web MIDI API](https://developer.mozilla.org/en-US/docs/Web/API/Web_MIDI_API)
- [Teensy Audio Library Source](https://github.com/PaulStoffregen/Audio)

## Questions?

This is a solid foundation. The approach is definitely **fathomable** - you're not rewriting the synth engine, just swapping out the platform layer. The audio logic, chord calculations, and musical behavior all stay the same.

Good luck! 🎹✨
