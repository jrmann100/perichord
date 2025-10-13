#include "Audio.h"
#include <emscripten.h>
#include <stdio.h>

// Static member initialization
int AudioStream::next_object_id = 1;

// Audio memory management stubs
void AudioMemory(int num)
{
    // In WASM, memory is managed by JavaScript Web Audio API
    printf("AudioMemory(%d) - managed by Web Audio\n", num);
}

void AudioNoInterrupts()
{
    // In WASM, this is a no-op - no actual interrupts to disable
}

void AudioInterrupts()
{
    // In WASM, this is a no-op
}

// AudioStream implementation
AudioStream::AudioStream(unsigned char ninput, unsigned char noutput)
    : num_inputs(ninput), num_outputs(noutput)
{
    object_id = next_object_id++;

    // Notify JavaScript to create corresponding Web Audio node
    const char *type = "AudioStream";
    js_audio_create_object(object_id, type);
}

AudioStream::~AudioStream()
{
    // Cleanup handled by JavaScript garbage collection
}

// AudioConnection implementation
AudioConnection::AudioConnection(AudioStream &source, AudioStream &destination)
    : src(&source), dst(&destination), src_index(0), dst_index(0)
{
    js_audio_connect(src->object_id, src_index, dst->object_id, dst_index);
}

AudioConnection::AudioConnection(AudioStream &source, unsigned char sourceOutput,
                                 AudioStream &destination, unsigned char destinationInput)
    : src(&source), dst(&destination), src_index(sourceOutput), dst_index(destinationInput)
{
    js_audio_connect(src->object_id, src_index, dst->object_id, dst_index);
}

AudioConnection::~AudioConnection()
{
    // Disconnection handled by JavaScript
}

// AudioSynthWaveform implementation
void AudioSynthWaveform::update_js()
{
    js_audio_create_object(object_id, "AudioSynthWaveform");
    js_audio_set_param(object_id, "frequency", frequency_);
    js_audio_set_param(object_id, "amplitude", amplitude_);
    js_audio_set_param(object_id, "waveform", (float)waveform_);
    js_audio_set_param(object_id, "dutyCycle", duty_cycle_);
    js_audio_set_param(object_id, "phase", phase_);
}

// AudioSynthWaveformModulated implementation
void AudioSynthWaveformModulated::update_js()
{
    js_audio_create_object(object_id, "AudioSynthWaveformModulated");
    js_audio_set_param(object_id, "frequency", frequency_);
    js_audio_set_param(object_id, "amplitude", amplitude_);
    js_audio_set_param(object_id, "waveform", (float)waveform_);
    js_audio_set_param(object_id, "dutyCycle", duty_cycle_);
    js_audio_set_param(object_id, "phase", phase_);
}

// AudioSynthWaveformDc implementation
void AudioSynthWaveformDc::update_js()
{
    js_audio_create_object(object_id, "AudioSynthWaveformDc");
    js_audio_set_param(object_id, "amplitude", amplitude_);
    js_audio_set_param(object_id, "rampTime", ramp_time_);
}

// AudioSynthNoiseWhite implementation
void AudioSynthNoiseWhite::update_js()
{
    js_audio_create_object(object_id, "AudioSynthNoiseWhite");
    js_audio_set_param(object_id, "amplitude", amplitude_);
}

// AudioEffectEnvelope implementation
void AudioEffectEnvelope::update_js()
{
    js_audio_create_object(object_id, "AudioEffectEnvelope");
    js_audio_set_param(object_id, "attack", attack_);
    js_audio_set_param(object_id, "hold", hold_);
    js_audio_set_param(object_id, "decay", decay_);
    js_audio_set_param(object_id, "sustain", sustain_);
    js_audio_set_param(object_id, "release", release_);
    js_audio_set_param(object_id, "releaseNoteOn", release_note_on_);
    js_audio_set_param(object_id, "delay", delay_);
}

void AudioEffectEnvelope::update_js_trigger(bool on)
{
    update_js();
    is_active_ = on;
    js_audio_trigger(object_id, on ? "noteOn" : "noteOff", on ? 1 : 0);
}

// AudioFilterStateVariable implementation
void AudioFilterStateVariable::update_js()
{
    js_audio_create_object(object_id, "AudioFilterStateVariable");
    js_audio_set_param(object_id, "frequency", frequency_);
    js_audio_set_param(object_id, "resonance", resonance_);
    js_audio_set_param(object_id, "octaveControl", octave_control_);
}

// AudioMixer4 implementation
void AudioMixer4::update_js()
{
    js_audio_create_object(object_id, "AudioMixer4");
    for (int i = 0; i < 4; i++)
    {
        char param_name[16];
        snprintf(param_name, sizeof(param_name), "gain%d", i);
        js_audio_set_param(object_id, param_name, gains_[i]);
    }
}

// AudioAmplifier implementation
void AudioAmplifier::update_js()
{
    js_audio_create_object(object_id, "AudioAmplifier");
    js_audio_set_param(object_id, "gain", gain_);
}

// AudioEffectDelay implementation
void AudioEffectDelay::update_js()
{
    js_audio_create_object(object_id, "AudioEffectDelay");
    for (int i = 0; i < 8; i++)
    {
        char param_name[16];
        snprintf(param_name, sizeof(param_name), "delay%d", i);
        js_audio_set_param(object_id, param_name, delays_[i]);
    }
}

// AudioEffectWaveshaper implementation
void AudioEffectWaveshaper::update_js()
{
    js_audio_create_object(object_id, "AudioEffectWaveshaper");
    if (waveshape_data_ && waveshape_length_ > 0)
    {
        js_audio_set_array(object_id, "curve", waveshape_data_, waveshape_length_);
    }
}
