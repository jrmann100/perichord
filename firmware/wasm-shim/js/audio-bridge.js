/**
 * Web Audio Bridge for MiniChord WASM
 *
 * This JavaScript module creates and manages Web Audio API nodes
 * corresponding to the C++ Audio objects in the WASM module.
 */

class AudioBridge {
  constructor() {
    this.audioContext = null;
    this.audioObjects = new Map(); // Maps object_id to Web Audio nodes
    this.connections = [];
    this.startTime = performance.now();
  }

  /**
   * Initialize the Web Audio context
   */
  async init() {
    this.audioContext = new (window.AudioContext || window.webkitAudioContext)({
      sampleRate: 44100,
      latencyHint: "interactive",
    });

    // Resume context on user interaction (required by browsers)
    if (this.audioContext.state === "suspended") {
      await this.audioContext.resume();
    }

    console.log(
      "AudioBridge initialized, sample rate:",
      this.audioContext.sampleRate
    );
    return this.audioContext;
  }

  /**
   * Create a Web Audio node based on the C++ audio object type
   */
  createObject(objectId, type) {
    if (this.audioObjects.has(objectId)) {
      return this.audioObjects.get(objectId);
    }

    let node = null;
    const ctx = this.audioContext;

    switch (type) {
      case "AudioSynthWaveform":
      case "AudioSynthWaveformModulated":
        node = {
          oscillator: ctx.createOscillator(),
          gain: ctx.createGain(),
          type: type,
          params: {
            frequency: 440,
            amplitude: 1.0,
            waveform: 0, // WAVEFORM_SINE
          },
        };
        node.oscillator.connect(node.gain);
        node.oscillator.start();
        node.output = node.gain;
        break;

      case "AudioSynthWaveformDc":
        node = {
          constantSource: ctx.createConstantSource(),
          gain: ctx.createGain(),
          type: type,
          params: { amplitude: 0 },
        };
        node.constantSource.connect(node.gain);
        node.constantSource.start();
        node.output = node.gain;
        break;

      case "AudioSynthNoiseWhite":
        node = this.createWhiteNoise();
        break;

      case "AudioEffectEnvelope":
        node = {
          gain: ctx.createGain(),
          type: type,
          params: {
            attack: 10.5,
            hold: 2.5,
            decay: 35,
            sustain: 0.5,
            release: 300,
          },
          isActive: false,
        };
        node.gain.gain.value = 0;
        node.input = node.gain;
        node.output = node.gain;
        break;

      case "AudioFilterStateVariable":
        node = {
          lowpass: ctx.createBiquadFilter(),
          highpass: ctx.createBiquadFilter(),
          bandpass: ctx.createBiquadFilter(),
          splitter: ctx.createChannelSplitter(3),
          type: type,
          params: {
            frequency: 1000,
            resonance: 0.707,
          },
        };
        // Configure filter types
        node.lowpass.type = "lowpass";
        node.highpass.type = "highpass";
        node.bandpass.type = "bandpass";

        // Create input splitter
        node.input = ctx.createGain();
        node.input.connect(node.lowpass);
        node.input.connect(node.highpass);
        node.input.connect(node.bandpass);

        // Output 0: lowpass, 1: bandpass, 2: highpass
        node.outputs = [node.lowpass, node.bandpass, node.highpass];
        break;

      case "AudioMixer4":
        node = {
          mixer: ctx.createGain(),
          inputs: [],
          type: type,
          params: { gain0: 1, gain1: 1, gain2: 1, gain3: 1 },
        };
        // Create 4 input gains
        for (let i = 0; i < 4; i++) {
          const inputGain = ctx.createGain();
          inputGain.connect(node.mixer);
          node.inputs.push(inputGain);
        }
        node.output = node.mixer;
        break;

      case "AudioAmplifier":
        node = {
          gain: ctx.createGain(),
          type: type,
          params: { gain: 1.0 },
        };
        node.input = node.gain;
        node.output = node.gain;
        break;

      case "AudioEffectDelay":
        node = {
          delays: [],
          gains: [],
          merger: ctx.createChannelMerger(8),
          type: type,
          params: {},
        };
        node.input = ctx.createGain();
        // Create 8 delay lines
        for (let i = 0; i < 8; i++) {
          const delay = ctx.createDelay(5.0); // Max 5 seconds
          const gain = ctx.createGain();
          node.input.connect(delay);
          delay.connect(gain);
          gain.connect(node.merger, 0, i);
          node.delays.push(delay);
          node.gains.push(gain);
          node.params[`delay${i}`] = 0;
        }
        node.output = node.merger;
        break;

      case "AudioEffectMultiply":
        // Ring modulation - not natively supported, would need AudioWorklet
        node = {
          gain: ctx.createGain(), // Placeholder
          type: type,
          params: {},
        };
        node.input = node.gain;
        node.output = node.gain;
        console.warn(
          "AudioEffectMultiply: Using placeholder (needs AudioWorklet)"
        );
        break;

      case "AudioEffectWaveshaper":
        node = {
          waveshaper: ctx.createWaveShaper(),
          type: type,
          params: {},
        };
        node.input = node.waveshaper;
        node.output = node.waveshaper;
        break;

      case "AudioOutputUSB":
      case "AudioOutputI2S":
        node = {
          type: type,
          destination: ctx.destination,
          output: ctx.destination,
        };
        break;

      case "AudioInputI2S":
        // Would connect to microphone input
        node = {
          type: type,
          params: {},
        };
        console.warn("AudioInputI2S: Not implemented");
        break;

      default:
        console.warn(`Unknown audio object type: ${type}`);
        node = {
          type: type,
          gain: ctx.createGain(),
          params: {},
        };
        node.input = node.gain;
        node.output = node.gain;
    }

    this.audioObjects.set(objectId, node);
    console.log(`Created ${type} with ID ${objectId}`);
    return node;
  }

  /**
   * Create white noise generator using AudioWorklet or fallback
   */
  createWhiteNoise() {
    const ctx = this.audioContext;
    const bufferSize = 4096;
    const buffer = ctx.createBuffer(1, bufferSize, ctx.sampleRate);
    const data = buffer.getChannelData(0);

    // Fill with white noise
    for (let i = 0; i < bufferSize; i++) {
      data[i] = Math.random() * 2 - 1;
    }

    const source = ctx.createBufferSource();
    source.buffer = buffer;
    source.loop = true;

    const gain = ctx.createGain();
    source.connect(gain);
    source.start();

    return {
      source: source,
      gain: gain,
      type: "AudioSynthNoiseWhite",
      params: { amplitude: 1.0 },
      output: gain,
    };
  }

  /**
   * Connect two audio objects
   */
  connect(srcId, srcOutput, dstId, dstInput) {
    const src = this.audioObjects.get(srcId);
    let dst = this.audioObjects.get(dstId);

    // Auto-create destination if it doesn't exist and is an output node
    if (!dst && dstId === 9999) {
      console.log("Auto-creating USB output destination");
      dst = this.createObject(dstId, "AudioOutputUSB");
    }

    if (!src || !dst) {
      console.warn(
        `Cannot connect: src=${srcId} (${
          src ? "exists" : "missing"
        }) dst=${dstId} (${dst ? "exists" : "missing"})`
      );
      return;
    }

    try {
      // Get the appropriate output node
      let outputNode = src.output;
      if (src.outputs && src.outputs[srcOutput]) {
        outputNode = src.outputs[srcOutput];
      }

      // Get the appropriate input node
      let inputNode = dst.input || dst.destination || dst.output;
      if (dst.inputs && dst.inputs[dstInput]) {
        inputNode = dst.inputs[dstInput];
      }

      if (outputNode && inputNode) {
        outputNode.connect(inputNode);
        this.connections.push({ srcId, srcOutput, dstId, dstInput });
        console.log(`Connected ${srcId}:${srcOutput} -> ${dstId}:${dstInput}`);
      }
    } catch (e) {
      console.error(`Connection error:`, e);
    }
  }

  /**
   * Set a parameter on an audio object
   */
  setParam(objectId, paramName, value) {
    const obj = this.audioObjects.get(objectId);
    if (!obj) return;

    obj.params = obj.params || {};
    obj.params[paramName] = value;

    // Apply parameter based on object type
    switch (obj.type) {
      case "AudioSynthWaveform":
      case "AudioSynthWaveformModulated":
        if (paramName === "frequency") {
          obj.oscillator.frequency.setValueAtTime(
            value,
            this.audioContext.currentTime
          );
        } else if (paramName === "amplitude") {
          obj.gain.gain.setValueAtTime(value, this.audioContext.currentTime);
        } else if (paramName === "waveform") {
          const waveforms = ["sine", "sawtooth", "square", "triangle"];
          obj.oscillator.type = waveforms[Math.floor(value)] || "sine";
        }
        break;

      case "AudioSynthWaveformDc":
        if (paramName === "amplitude") {
          obj.constantSource.offset.setValueAtTime(
            value,
            this.audioContext.currentTime
          );
        }
        break;

      case "AudioSynthNoiseWhite":
        if (paramName === "amplitude") {
          obj.gain.gain.setValueAtTime(value, this.audioContext.currentTime);
        }
        break;

      case "AudioFilterStateVariable":
        if (paramName === "frequency") {
          obj.lowpass.frequency.setValueAtTime(
            value,
            this.audioContext.currentTime
          );
          obj.highpass.frequency.setValueAtTime(
            value,
            this.audioContext.currentTime
          );
          obj.bandpass.frequency.setValueAtTime(
            value,
            this.audioContext.currentTime
          );
        } else if (paramName === "resonance") {
          const Q = value * 30; // Scale to reasonable Q range
          obj.lowpass.Q.setValueAtTime(Q, this.audioContext.currentTime);
          obj.highpass.Q.setValueAtTime(Q, this.audioContext.currentTime);
          obj.bandpass.Q.setValueAtTime(Q, this.audioContext.currentTime);
        }
        break;

      case "AudioMixer4":
        const match = paramName.match(/gain(\d)/);
        if (match) {
          const index = parseInt(match[1]);
          if (obj.inputs[index]) {
            obj.inputs[index].gain.setValueAtTime(
              value,
              this.audioContext.currentTime
            );
          }
        }
        break;

      case "AudioAmplifier":
        if (paramName === "gain") {
          obj.gain.gain.setValueAtTime(value, this.audioContext.currentTime);
        }
        break;

      case "AudioEffectDelay":
        const delayMatch = paramName.match(/delay(\d)/);
        if (delayMatch) {
          const index = parseInt(delayMatch[1]);
          if (obj.delays[index]) {
            obj.delays[index].delayTime.setValueAtTime(
              value / 1000, // Convert ms to seconds
              this.audioContext.currentTime
            );
          }
        }
        break;
    }
  }

  /**
   * Trigger an event on an audio object (e.g., envelope noteOn/noteOff)
   */
  trigger(objectId, eventName, value) {
    const obj = this.audioObjects.get(objectId);
    if (!obj) return;

    const now = this.audioContext.currentTime;

    if (obj.type === "AudioEffectEnvelope") {
      if (eventName === "noteOn") {
        const p = obj.params;
        const gain = obj.gain.gain;

        gain.cancelScheduledValues(now);
        gain.setValueAtTime(0, now);

        // Attack
        gain.linearRampToValueAtTime(1.0, now + p.attack / 1000);

        // Hold
        const holdEnd = now + (p.attack + p.hold) / 1000;
        gain.setValueAtTime(1.0, holdEnd);

        // Decay to sustain
        gain.linearRampToValueAtTime(p.sustain, holdEnd + p.decay / 1000);

        obj.isActive = true;
      } else if (eventName === "noteOff") {
        const p = obj.params;
        const gain = obj.gain.gain;
        const currentValue = gain.value;

        gain.cancelScheduledValues(now);
        gain.setValueAtTime(currentValue, now);
        gain.linearRampToValueAtTime(0, now + p.release / 1000);

        obj.isActive = false;
      }
    }
  }

  /**
   * Set an array parameter (e.g., waveshaper curve)
   */
  setArray(objectId, paramName, dataPtr, length) {
    const obj = this.audioObjects.get(objectId);
    if (!obj) return;

    // Read float array from WASM memory
    const data = new Float32Array(Module.HEAPF32.buffer, dataPtr, length);

    if (obj.type === "AudioEffectWaveshaper" && paramName === "curve") {
      obj.waveshaper.curve = new Float32Array(data);
    }
  }
}

// Global audio bridge instance
const audioBridge = new AudioBridge();

// Export functions for WASM to call
window.audioBridgeAPI = {
  init: () => audioBridge.init(),
  createObject: (id, type) => audioBridge.createObject(id, UTF8ToString(type)),
  connect: (srcId, srcOut, dstId, dstIn) =>
    audioBridge.connect(srcId, srcOut, dstId, dstIn),
  setParam: (id, param, value) =>
    audioBridge.setParam(id, UTF8ToString(param), value),
  trigger: (id, event, value) =>
    audioBridge.trigger(id, UTF8ToString(event), value),
  setArray: (id, param, dataPtr, len) =>
    audioBridge.setArray(id, UTF8ToString(param), dataPtr, len),

  // Timing functions for Arduino shim
  performanceNow: () => performance.now(),
  consoleLog: (msgPtr) => console.log(UTF8ToString(msgPtr)),

  // Pin I/O stubs (can be connected to virtual controls or MIDI)
  setPin: (pin, value) => {
    /* TODO: virtual control */
  },
  getPin: (pin) => 0, // TODO: virtual control

  // Timer functions (would use setTimeout/setInterval)
  setTimer: (id, microseconds) => {
    /* TODO */
  },
  clearTimer: (id) => {
    /* TODO */
  },
};

// Helper to convert C string pointer to JS string
function UTF8ToString(ptr) {
  if (!ptr) return "";
  const heap = new Uint8Array(Module.HEAPU8.buffer);
  let str = "";
  let i = ptr;
  while (heap[i]) {
    str += String.fromCharCode(heap[i++]);
  }
  return str;
}

export default audioBridge;
