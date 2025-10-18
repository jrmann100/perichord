const powerButton = document.getElementById("power");
powerButton.disabled = true;

const perichord = await (await import("./dist/perichord.js")).default();
const worklet = new perichord.PerichordAudioWorklet(() => {
  powerButton.disabled = false;
});
const audioContext = perichord.emscriptenGetAudioObject(worklet.audioContext);
// await audioContext.suspend(); // autoplay is sometimes on in localhost
audioContext.addEventListener("statechange", () => {
  powerButton.textContent =
    audioContext.state === "running" ? "Power Off" : "Power On";
});

powerButton.addEventListener("click", async () => {
  if (audioContext.state === "running") {
    await audioContext.suspend();
  } else {
    await audioContext.resume();
  }
});
