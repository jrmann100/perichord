import perichordModule from "./dist/perichord.js";
import * as def from "./def.js";

const HIGH = 1;
const LOW = 0;
const INPUT = 0;

const els = Object.fromEntries(
  Object.entries({
    powerButton: "#power-button",
    rhythmLed: "#rhythm-led",
    holdButton: "#hold-button",
    chordMatrixButtons: ".chord-matrix-button",
    rgbLed: "#rgb-led",
    chordPot: "#chord-pot",
    harpPot: "#harp-pot",
    modPot: "#mod-pot",
    upPgmButton: "#up-pgm-button",
    downPgmButton: "#down-pgm-button",
  }).map(([key, selector]) => {
    const result = selector.startsWith("#")
      ? document.querySelector(selector)
      : Array.from(document.querySelectorAll(selector));
    if (result === null || (Array.isArray(result) && result.length === 0)) {
      throw new Error(`No matches for ${selector}`);
    }
    return [key, result];
  })
);

const disabled = (el, state) => el.setAttribute("aria-disabled", state);

[
  els.holdButton,
  ...els.chordMatrixButtons,
  els.upPgmButton,
  els.downPgmButton,
].forEach((el) => disabled(el, true));

const perichord = await perichordModule();
const worklet = new perichord.PerichordAudioWorklet(() => {
  els.powerButton.disabled = false;
});
const audioContext = perichord.emscriptenGetAudioObject(worklet.audioContext);
// await audioContext.suspend(); // autoplay is sometimes on in localhost
audioContext.addEventListener("statechange", () => {
  els.powerButton.textContent = audioContext.state === "running" ? "Off" : "On";
});

els.powerButton.addEventListener("click", async () => {
  if (audioContext.state === "running") {
    await audioContext.suspend();
  } else {
    await audioContext.resume();
  }
});

const powerLED = new (class {
  #r = 0;
  #g = 0;
  #b = 0;

  set r(value) {
    this.#r = value;
    this.update();
  }
  set g(value) {
    this.#g = value;
    this.update();
  }
  set b(value) {
    this.#b = value;
    this.update();
  }

  get hsv() {
    // literally just reverses the work done in main.cpp's set_led_color
    const r = this.#r / 200;
    const g = this.#g / 115;
    const b = this.#b / 70;

    // Find max and min values
    const max = Math.max(r, g, b);
    const min = Math.min(r, g, b);
    const delta = max - min;

    // Calculate V (value)
    const v = max;

    // Calculate S (saturation)
    let s = 0;
    if (max > 0) {
      s = delta / max;
    }

    // Calculate H (hue)
    let h = 0;
    if (delta > 0) {
      if (max === r) {
        h = 60 * (((g - b) / delta) % 6);
      } else if (max === g) {
        h = 60 * ((b - r) / delta + 2);
      } else {
        // max === b
        h = 60 * ((r - g) / delta + 4);
      }
    }

    // Normalize hue to [0, 360)
    if (h < 0) {
      h += 360;
    }

    return { h, s, v };
  }

  update() {
    const { h, s, v } = this.hsv;
    els.rgbLed.style.fill = `hsl(${h}, ${s * 100}%, ${v * 60}%)`;
  }
})();

perichord.addEventListener("analogwrite", (e) => {
  const { pin, value } = e.detail;
  if (pin === def.R_LED_PIN) {
    powerLED.r = value;
  }
  if (pin === def.G_LED_PIN) {
    powerLED.g = value;
  }
  if (pin === def.B_LED_PIN) {
    powerLED.b = value;
  }
  if (pin === def.RYTHM_LED_PIN) {
    els.rhythmLed.style.fill = value > 0 ? "hsl(0, 100%, 60%)" : "black";
  }
});

class Pin {
  #ptr;
  constructor(pin, ptr) {
    let button = null;
    if (pin === def.DOWN_PGM_PIN) {
      button = els.downPgmButton;
    } else if (pin === def.UP_PGM_PIN) {
      button = els.upPgmButton;
    } else if (pin === def.HOLD_BUTTON_PIN) {
      button = els.holdButton;
    }
    if (button) {
      disabled(button, false);
      button.addEventListener("mousedown", () => this.mousedown());
      // todo: somewhat inefficient to add global mouseup listener for each pin
      document.addEventListener("mouseup", () => this.mouseup());
    } else {
      console.warn(`Pin ${pin} not handled`);
    }
    this.#ptr = ptr;
  }
  set value(value) {
    const view = new DataView(perichord.wasmMemory.buffer, this.#ptr, 4);
    view.setInt32(0, value, true);
  }

  mousedown() {
    this.value = HIGH;
  }
  mouseup() {
    this.value = LOW;
  }
}

perichord.addEventListener("pinmode", ({ detail: { pin, mode, ptr } }) => {
  if (mode !== INPUT) {
    return;
  }
  new Pin(pin, ptr);
});

perichord.addEventListener("buttonmatrix-setup", (e) => {
  // TODO: I believe you have to init dataview every time in case memory grows;
  // we might later just fix memory size once we're sure we don't need to grow it
  // and then we can cache one big dataview and just index into it.
  els.chordMatrixButtons.forEach((button, index) => {
    disabled(button, false);
    const setTo = (value) => () => {
      const view = new DataView(
        perichord.wasmMemory.buffer,
        e.detail + index,
        1
      );
      view.setUint8(0, value);
    };
    button.addEventListener("mousedown", setTo(HIGH));
    button.addEventListener("mouseup", setTo(LOW));
  });
});
