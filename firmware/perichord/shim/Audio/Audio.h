#ifndef Audio_h_
#define Audio_h_

#include <AudioStream.h>
#include <math.h>

// When changing multiple audio object settings that must update at
// the same time, these functions allow the audio library interrupt
// to be disabled.  For example, you may wish to begin playing a note
// in response to reading an analog sensor.  If you have "velocity"
// information, you might start the sample playing and also adjust
// the gain of a mixer channel.  Use AudioNoInterrupts() first, then
// make both changes to the 2 separate objects.  Then allow the audio
// library to update with AudioInterrupts().  Both changes will happen
// at the same time, because AudioNoInterrupts() prevents any updates
// while you make changes.
//
// #define AudioNoInterrupts() (NVIC_DISABLE_IRQ(IRQ_SOFTWARE))
// #define AudioInterrupts() (NVIC_ENABLE_IRQ(IRQ_SOFTWARE))
// TODO: not sure how to implement this in emscripten. is it just as simple as pausing the audio loop?
#define AudioNoInterrupts() ((void)0)
#define AudioInterrupts() ((void)0)

// include all the library headers, so a sketch can use a single
// #include <Audio.h> to get the whole library
//
// #include "analyze_fft1024.h"
// #include "analyze_fft256.h"
// #include "analyze_notefreq.h"
// #include "analyze_peak.h"
// #include "analyze_print.h"
// #include "analyze_rms.h"
// #include "analyze_tonedetect.h"
// #include "async_input_spdif3.h"
// #include "control_ak4558.h"
// #include "control_cs42448.h"
// #include "control_cs4272.h"
// #include "control_sgtl5000.h"
// #include "control_tlv320aic3206.h"
// #include "control_wm8731.h"
// #include "effect_bitcrusher.h"
// #include "effect_chorus.h"
// #include "effect_combine.h"
#include "effect_delay.h"
// #include "effect_delay_ext.h"
#include "effect_envelope.h"
// #include "effect_fade.h"
// #include "effect_flange.h"
// #include "effect_freeverb.h"
// #include "effect_granular.h"
// #include "effect_midside.h"
#include "effect_multiply.h"
// #include "effect_rectifier.h"
// #include "effect_reverb.h"
// #include "effect_wavefolder.h"
#include "effect_waveshaper.h"
// #include "filter_biquad.h"
// #include "filter_fir.h"
// #include "filter_ladder.h"
#include "filter_variable.h"
// #include "input_adc.h"
// #include "input_adcs.h"
// #include "input_i2s.h"
// #include "input_i2s2.h"
// #include "input_i2s_hex.h"
// #include "input_i2s_oct.h"
// #include "input_i2s_quad.h"
// #include "input_pdm.h"
// #include "input_pdm_i2s2.h"
// #include "input_spdif3.h"
// #include "input_tdm.h"
// #include "input_tdm2.h"
#include "mixer.h"
// #include "output_adat.h"
// #include "output_dac.h"
// #include "output_dacs.h"
#include "output_i2s.h"
// #include "output_i2s2.h"
// #include "output_i2s_hex.h"
// #include "output_i2s_oct.h"
// #include "output_i2s_quad.h"
// #include "output_mqs.h"
// #include "output_pt8211.h"
// #include "output_pt8211_2.h"
// #include "output_pwm.h"
// #include "output_spdif.h"
// #include "output_spdif2.h"
// #include "output_spdif3.h"
// #include "output_tdm.h"
// #include "output_tdm2.h"
// #include "play_memory.h"
// #include "play_queue.h"
// #include "play_sd_raw.h"
// #include "play_sd_wav.h"
// #include "play_serialflash_raw.h"
// #include "record_queue.h"
#include "synth_dc.h"
// #include "synth_karplusstrong.h"
// #include "synth_pinknoise.h"
// #include "synth_pwm.h"
// #include "synth_simple_drum.h"
// #include "synth_sine.h"
// #include "synth_tonesweep.h"
#include "synth_waveform.h"
// #include "synth_wavetable.h"
#include "synth_whitenoise.h"

class AudioInputI2S : public AudioStream
{
public:
    AudioInputI2S() : AudioStream(0, NULL) {}
    virtual void update(void) {}
};

// TODO: eventually this is probably what we will want to hack into web audio output
class AudioOutputUSB : public AudioStream
{
public:
    AudioOutputUSB() : AudioStream(2, inputQueueArray) {}
    virtual void update(void) {}

private:
    audio_block_t *inputQueueArray[2];
};

#endif