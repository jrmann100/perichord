#ifndef AUDIO_WASM_SHIM_H
#define AUDIO_WASM_SHIM_H

#include <stdint.h>
#include <string.h>

// Audio configuration
#define AUDIO_BLOCK_SAMPLES 128
#define AUDIO_SAMPLE_RATE_EXACT 44100.0f

// Forward declarations
class AudioStream;
class AudioConnection;

// Audio memory management
void AudioMemory(int num);
void AudioNoInterrupts();
void AudioInterrupts();

// Base class for all audio objects
class AudioStream
{
public:
    AudioStream(unsigned char ninput, unsigned char noutput);
    virtual ~AudioStream();

    virtual void update() {}

    unsigned char num_inputs;
    unsigned char num_outputs;
    int object_id; // Unique ID for JavaScript bridge

protected:
    static int next_object_id;
};

// Audio connection - represents cable between audio objects
class AudioConnection
{
public:
    AudioConnection(AudioStream &source, AudioStream &destination);
    AudioConnection(AudioStream &source, unsigned char sourceOutput,
                    AudioStream &destination, unsigned char destinationInput);
    ~AudioConnection();

private:
    AudioStream *src;
    AudioStream *dst;
    unsigned char src_index;
    unsigned char dst_index;
};

// Waveform types (matching Teensy Audio Library)
#define WAVEFORM_SINE 0
#define WAVEFORM_SAWTOOTH 1
#define WAVEFORM_SQUARE 2
#define WAVEFORM_TRIANGLE 3
#define WAVEFORM_ARBITRARY 4
#define WAVEFORM_PULSE 5
#define WAVEFORM_SAWTOOTH_REVERSE 6
#define WAVEFORM_SAMPLE_HOLD 7
#define WAVEFORM_TRIANGLE_VARIABLE 8
#define WAVEFORM_BANDLIMIT_SAWTOOTH 9
#define WAVEFORM_BANDLIMIT_SAWTOOTH_REVERSE 10
#define WAVEFORM_BANDLIMIT_SQUARE 11
#define WAVEFORM_BANDLIMIT_PULSE 12

// Basic waveform generator
class AudioSynthWaveform : public AudioStream
{
public:
    AudioSynthWaveform() : AudioStream(0, 1)
    {
        frequency_ = 440.0f;
        amplitude_ = 1.0f;
        waveform_ = WAVEFORM_SINE;
    }

    void frequency(float freq)
    {
        frequency_ = freq;
        update_js();
    }
    void amplitude(float amp)
    {
        amplitude_ = amp;
        update_js();
    }
    void begin(short waveform)
    {
        waveform_ = waveform;
        update_js();
    }
    void begin(float duty, short waveform)
    {
        duty_cycle_ = duty;
        waveform_ = waveform;
        update_js();
    }
    void pulseWidth(float duty)
    {
        duty_cycle_ = duty;
        update_js();
    }
    void phase(float angle)
    {
        phase_ = angle;
        update_js();
    }

private:
    float frequency_;
    float amplitude_;
    short waveform_;
    float duty_cycle_ = 0.5f;
    float phase_ = 0.0f;
    void update_js();
};

// Modulated waveform (for FM/PM)
class AudioSynthWaveformModulated : public AudioStream
{
public:
    AudioSynthWaveformModulated() : AudioStream(1, 1)
    {
        frequency_ = 440.0f;
        amplitude_ = 1.0f;
        waveform_ = WAVEFORM_SINE;
    }

    void frequency(float freq)
    {
        frequency_ = freq;
        update_js();
    }
    void amplitude(float amp)
    {
        amplitude_ = amp;
        update_js();
    }
    void begin(short waveform)
    {
        waveform_ = waveform;
        update_js();
    }
    void begin(float duty, short waveform)
    {
        duty_cycle_ = duty;
        waveform_ = waveform;
        update_js();
    }
    void pulseWidth(float duty)
    {
        duty_cycle_ = duty;
        update_js();
    }
    void phase(float angle)
    {
        phase_ = angle;
        update_js();
    }

private:
    float frequency_;
    float amplitude_;
    short waveform_;
    float duty_cycle_ = 0.5f;
    float phase_ = 0.0f;
    void update_js();
};

// DC offset generator
class AudioSynthWaveformDc : public AudioStream
{
public:
    AudioSynthWaveformDc() : AudioStream(0, 1), amplitude_(0.0f) {}

    void amplitude(float amp)
    {
        amplitude_ = amp;
        update_js();
    }
    void amplitude(float amp, float ramp_time)
    {
        amplitude_ = amp;
        ramp_time_ = ramp_time;
        update_js();
    }

private:
    float amplitude_;
    float ramp_time_ = 0.0f;
    void update_js();
};

// White noise generator
class AudioSynthNoiseWhite : public AudioStream
{
public:
    AudioSynthNoiseWhite() : AudioStream(0, 1), amplitude_(1.0f) {}

    void amplitude(float amp)
    {
        amplitude_ = amp;
        update_js();
    }

private:
    float amplitude_;
    void update_js();
};

// Envelope generator
class AudioEffectEnvelope : public AudioStream
{
public:
    AudioEffectEnvelope() : AudioStream(1, 1)
    {
        attack_ = 10.5f;
        hold_ = 2.5f;
        decay_ = 35.0f;
        sustain_ = 0.5f;
        release_ = 300.0f;
    }

    void noteOn() { update_js_trigger(true); }
    void noteOff() { update_js_trigger(false); }
    void attack(float ms)
    {
        attack_ = ms;
        update_js();
    }
    void hold(float ms)
    {
        hold_ = ms;
        update_js();
    }
    void decay(float ms)
    {
        decay_ = ms;
        update_js();
    }
    void sustain(float level)
    {
        sustain_ = level;
        update_js();
    }
    void release(float ms)
    {
        release_ = ms;
        update_js();
    }
    void releaseNoteOn(float ms)
    {
        release_note_on_ = ms;
        update_js();
    }
    void delay(float ms)
    {
        delay_ = ms;
        update_js();
    }
    bool isActive() { return is_active_; }

private:
    float attack_;
    float hold_;
    float decay_;
    float sustain_;
    float release_;
    float release_note_on_ = 0.0f;
    float delay_ = 0.0f;
    bool is_active_ = false;
    void update_js();
    void update_js_trigger(bool on);
};

// State variable filter
class AudioFilterStateVariable : public AudioStream
{
public:
    AudioFilterStateVariable() : AudioStream(2, 3)
    {
        frequency_ = 1000.0f;
        resonance_ = 0.707f;
    }

    void frequency(float freq)
    {
        frequency_ = freq;
        update_js();
    }
    void resonance(float q)
    {
        resonance_ = q;
        update_js();
    }
    void octaveControl(float oct)
    {
        octave_control_ = oct;
        update_js();
    }

private:
    float frequency_;
    float resonance_;
    float octave_control_ = 0.0f;
    void update_js();
};

// 4-channel mixer
class AudioMixer4 : public AudioStream
{
public:
    AudioMixer4() : AudioStream(4, 1)
    {
        for (int i = 0; i < 4; i++)
            gains_[i] = 1.0f;
    }

    void gain(unsigned int channel, float level)
    {
        if (channel < 4)
        {
            gains_[channel] = level;
            update_js();
        }
    }

private:
    float gains_[4];
    void update_js();
};

// Amplifier/gain
class AudioAmplifier : public AudioStream
{
public:
    AudioAmplifier() : AudioStream(1, 1), gain_(1.0f) {}

    void gain(float level)
    {
        gain_ = level;
        update_js();
    }

private:
    float gain_;
    void update_js();
};

// Delay effect
class AudioEffectDelay : public AudioStream
{
public:
    AudioEffectDelay() : AudioStream(1, 8)
    {
        for (int i = 0; i < 8; i++)
            delays_[i] = 0.0f;
    }

    void delay(uint8_t channel, float ms)
    {
        if (channel < 8)
        {
            delays_[channel] = ms;
            update_js();
        }
    }

private:
    float delays_[8];
    void update_js();
};

// Multiply effect (ring modulation)
class AudioEffectMultiply : public AudioStream
{
public:
    AudioEffectMultiply() : AudioStream(2, 1) {}
};

// Waveshaper/distortion
class AudioEffectWaveshaper : public AudioStream
{
public:
    AudioEffectWaveshaper() : AudioStream(1, 1), waveshape_data_(nullptr) {}

    void shape(float *data, int length)
    {
        waveshape_data_ = data;
        waveshape_length_ = length;
        update_js();
    }

private:
    float *waveshape_data_;
    int waveshape_length_ = 0;
    void update_js();
};

// USB audio output (Web Audio destination)
class AudioOutputUSB : public AudioStream
{
public:
    AudioOutputUSB() : AudioStream(2, 0) {}
};

// I2S audio output (DAC)
class AudioOutputI2S : public AudioStream
{
public:
    AudioOutputI2S() : AudioStream(2, 0) {}
};

// I2S audio input (ADC/microphone)
class AudioInputI2S : public AudioStream
{
public:
    AudioInputI2S() : AudioStream(0, 2) {}
};

// JavaScript bridge functions (implemented in JS)
extern "C"
{
    void js_audio_create_object(int object_id, const char *type);
    void js_audio_connect(int src_id, int src_output, int dst_id, int dst_input);
    void js_audio_set_param(int object_id, const char *param, float value);
    void js_audio_set_param_string(int object_id, const char *param, const char *value);
    void js_audio_trigger(int object_id, const char *event, int value);
    void js_audio_set_array(int object_id, const char *param, float *data, int length);
}

#endif // AUDIO_WASM_SHIM_H
