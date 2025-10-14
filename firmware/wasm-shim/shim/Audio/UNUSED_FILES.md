Audio shim: required vs unused files
===================================

Summary
-------
This project (firmware) only needs a small subset of the full PJRC
Teensy Audio library that lives in this directory.  To find the files
actually used by the firmware we started from the single header the
firmware includes (`Audio.h`) and computed the transitive closure of
all headers and implementation files reachable from that header.

Required (reachable) files
--------------------------
These files are referenced (directly or transitively) from `Audio.h`
and are kept:

- Audio.h
- AudioStream.h
- AudioStream.cpp (shim)
- effect_delay.h / effect_delay.cpp
- effect_envelope.h / effect_envelope.cpp
- effect_multiply.h / effect_multiply.cpp
- effect_waveshaper.h / effect_waveshaper.cpp
- filter_variable.h / filter_variable.cpp
- mixer.h / mixer.cpp
- synth_dc.h / synth_dc.cpp
- synth_waveform.h / synth_waveform.cpp
- synth_whitenoise.h / synth_whitenoise.cpp
- utility/dspinst.h
- arm_math.h (WASM stub)
- data_waveforms.c

All other files under this directory tree were not reachable from
`Audio.h` and are therefore candidates for removal (or moving to
an archive) when preparing a minimal/shrunk WASM shim.  Examples
of files/directories that are unused by the firmware:

- examples/   (all example sketches)
- extras/     (auxiliary scripts and tools, e.g. wav2sketch)
- analyze_*   (FFT / analysis utilities not used by firmware)
- control_*   (hardware codec control drivers not used by firmware)
- input_*     (ADC/I2S/PDM/SPDIF/TDM input modules not used)
- output_*    (hardware output drivers for various codecs)
- play_*      (playback helpers: play_sd_wav, play_serialflash_raw, etc.)
- record_*    (recording helpers and queues)
- filter_biquad.h/cpp, filter_fir.h/cpp, filter_ladder.h/cpp (unused filters)
- many hardware/port specific files (memcpy_audio.S, memcpy_audio.h,
  imxrt_hw.h, spi_interrupt.*, platform specific optimizations, etc.)

What "marking for removal" means here
------------------------------------
Instead of editing hundreds of source files to add header comments,
this document is the authoritative audit of which files are required
and which are candidates for removal.  The next safe steps are:

1. Create a branch and back up the original audio tree.
2. Remove or move the unused directories (examples/, extras/, input_*,
   output_* etc) into an `Audio/archived/` folder and run a full
   build/test cycle for the firmware and wasm artifacts.
3. If all tests pass, delete the archived files.

If you prefer each file to be individually marked (top-of-file
comment, or moved) I can apply that change; however that is a large
patch (hundreds of files).  Creating this single manifest keeps the
change minimal and makes it trivial to review and act upon.

Questions or next steps
----------------------
- Would you like me to (A) move all unused files into
  `wasm-shim/shim/Audio/archived/` in a single PR, or (B) insert a
  small "UNUSED - candidate for removal" header into each unused file?
  (Option B will be a large patch to many files.)
