import perichordModule from "./dist/perichord.js";
import * as def from "./def.js"

const HIGH = 1;
const LOW = 0;
const INPUT = 0;

const rgbled = document.getElementById("rgbled");
const rhythmled = document.getElementById("rhythmled");
const powerButton = document.getElementById("power");
const pgmUpButton = document.getElementById("pgm-up");
const pgmDownButton = document.getElementById("pgm-down");
const holdButton = document.getElementById("hold");

const perichord = await perichordModule();
const worklet = new perichord.PerichordAudioWorklet(() => {
  powerButton.disabled = false;
});
const audioContext = perichord.emscriptenGetAudioObject(worklet.audioContext);
// await audioContext.suspend(); // autoplay is sometimes on in localhost
audioContext.addEventListener("statechange", () => {
  powerButton.textContent =
    audioContext.state === "running" ? "Off" : "On";
});

powerButton.addEventListener("click", async () => {
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
      h = 60 * (((b - r) / delta) + 2);
    } else { // max === b
      h = 60 * (((r - g) / delta) + 4);
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
    rgbled.style.backgroundColor = `hsl(${h}, ${s * 100}%, ${v * 60}%)`
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
    rhythmled.style.backgroundColor = value > 0 ? 'hsl(0, 100%, 60%)' : 'black';
  }
});


class Pin {
  #ptr;
  constructor(pin, ptr) {
    let button = null;
    if (pin === def.DOWN_PGM_PIN) {
      button = pgmDownButton;
    } else if (pin === def.UP_PGM_PIN) {
      button = pgmUpButton;
    } else if (pin === def.HOLD_BUTTON_PIN) {
      button = holdButton;
    }
    if (button) {
      button.disabled = false;
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

    if (view.getInt32(0, true) === value) {
      return;
    }
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
