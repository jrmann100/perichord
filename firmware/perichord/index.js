const powerButton = document.getElementById("power");
powerButton.disabled = true;

const perichord = await (await import("./dist/perichord.js")).default();
const worklet = new perichord.PerichordAudioWorklet(() => {
  powerButton.disabled = false;
  document.getElementById("power").addEventListener("click", async () => {
    await worklet.resume();
  });
});
const audioContext = perichord.emscriptenGetAudioObject(worklet.audioContext);
await audioContext.suspend(); // autoplay is sometimes on in localhost
