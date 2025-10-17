// https://emscripten.org/docs/api_reference/wasm_audio_worklets.html

#include "perichord.h"
#include <emscripten/em_math.h>
#include <emscripten/webaudio.h>

// Global pointer to the output for the audio callback
static AudioOutputI2S *g_audioOutput = nullptr;

bool GenerateAudioFromTeensy(int numInputs, const AudioSampleFrame *inputs,
                             int numOutputs, AudioSampleFrame *outputs,
                             int numParams, const AudioParamFrame *params,
                             void *userData)
{
    // Call the Teensy Audio library update chain
    AudioStream::update_all();

    // Get the rendered audio from the I2S output
    if (g_audioOutput && numOutputs > 0)
    {
        const int16_t *leftChannel = g_audioOutput->getLeftChannel();
        const int16_t *rightChannel = g_audioOutput->getRightChannel();

        // Convert int16_t samples to float samples expected by WebAudio
        // WebAudio expects samples in range [-1.0, 1.0]
        int samplesPerChannel = outputs[0].samplesPerChannel;
        int numChannels = outputs[0].numberOfChannels;

        for (int i = 0; i < samplesPerChannel; ++i)
        {
            float leftSample = leftChannel[i] / 32768.0f;
            float rightSample = rightChannel[i] / 32768.0f;

            if (numChannels == 1)
            {
                // Mono: mix both channels
                outputs[0].data[i] = (leftSample + rightSample) * 0.5f;
            }
            else if (numChannels >= 2)
            {
                // Stereo
                outputs[0].data[i * 2] = leftSample;
                outputs[0].data[i * 2 + 1] = rightSample;
            }
        }
    }

    return true; // Keep the graph output going
}

void PerichordAudioWorklet::onAudioThreadInitialized(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData)
{
    auto *self = static_cast<PerichordAudioWorklet *>(userData);
    if (!success)
    {
        return; // Check browser console in a debug build for detailed errors
    }
    WebAudioWorkletProcessorCreateOptions opts = {
        .name = PERICHORD_AUDIO_PROCESSOR_NAME,
    };
    emscripten_create_wasm_audio_worklet_processor_async(audioContext, &opts, &PerichordAudioWorklet::onAudioWorkletProcessorCreated, userData);
}

void PerichordAudioWorklet::onAudioWorkletProcessorCreated(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData)
{
    auto *self = static_cast<PerichordAudioWorklet *>(userData);
    if (!success)
    {
        return; // Check browser console in a debug build for detailed errors
    }

    int outputChannelCounts[1] = {1};
    EmscriptenAudioWorkletNodeCreateOptions options = {
        .numberOfInputs = 0,
        .numberOfOutputs = 1,
        .outputChannelCounts = outputChannelCounts};

    // Create node
    EMSCRIPTEN_AUDIO_WORKLET_NODE_T wasmAudioWorklet = emscripten_create_wasm_audio_worklet_node(audioContext,
                                                                                                 PERICHORD_AUDIO_PROCESSOR_NAME, &options, &GenerateAudioFromTeensy, 0);

    // Connect it to audio context destination
    emscripten_audio_node_connect(wasmAudioWorklet, audioContext, 0, 0);

    // Call the ready callback - we're already on the main thread!
    if (self->readyCallback.as<bool>())
    {
        self->readyCallback();
    }
}

PerichordAudioWorklet::PerichordAudioWorklet(emscripten::val readyCallback)
    : readyCallback(readyCallback), noise1(nullptr), i2s1(nullptr), patchCord1(nullptr), patchCord2(nullptr)
{
    audioContext = emscripten_create_audio_context(0);
    emscripten_start_wasm_audio_worklet_thread_async(audioContext, audioThreadStack, sizeof(audioThreadStack),
                                                     &onAudioThreadInitialized, this);
}

void PerichordAudioWorklet::setup()
{
    // Allocate audio memory for Teensy Audio library
    static audio_block_t audioMemory[10];
    AudioStream::initialize_memory(audioMemory, 10);

    // Create the audio components (like in test.cpp)
    noise1 = new AudioSynthNoiseWhite();
    i2s1 = new AudioOutputI2S();

    // Connect white noise to both left and right channels
    patchCord1 = new AudioConnection(*noise1, 0, *i2s1, 0);
    patchCord2 = new AudioConnection(*noise1, 0, *i2s1, 1);

    // Set amplitude (0.0 to 1.0)
    noise1->amplitude(0.5);

    // Store the output globally for the audio callback
    g_audioOutput = i2s1;
}

bool PerichordAudioWorklet::resume()
{
    if (emscripten_audio_context_state(audioContext) != AUDIO_CONTEXT_STATE_RUNNING)
    {
        emscripten_resume_audio_context_sync(audioContext);
        return true;
    }
    return false;
}