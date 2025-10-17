// https://emscripten.org/docs/api_reference/wasm_audio_worklets.html

#include "perichord.h"
#include <emscripten/em_math.h>
#include <emscripten/webaudio.h>

bool GenerateNoise(int numInputs, const AudioSampleFrame *inputs,
                   int numOutputs, AudioSampleFrame *outputs,
                   int numParams, const AudioParamFrame *params,
                   void *userData)
{
    for (int i = 0; i < numOutputs; ++i)
        for (int j = 0; j < outputs[i].samplesPerChannel * outputs[i].numberOfChannels; ++j)
            outputs[i].data[j] = emscripten_random() * 0.2 - 0.1; // Warning: scale down audio volume by factor of 0.2, raw noise can be really loud otherwise

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
                                                                                                 PERICHORD_AUDIO_PROCESSOR_NAME, &options, &GenerateNoise, 0);

    // Connect it to audio context destination
    emscripten_audio_node_connect(wasmAudioWorklet, audioContext, 0, 0);

    // Call the ready callback - we're already on the main thread!
    if (self->readyCallback.as<bool>())
    {
        self->readyCallback();
    }
}

PerichordAudioWorklet::PerichordAudioWorklet(emscripten::val readyCallback)
    : readyCallback(readyCallback)
{
    audioContext = emscripten_create_audio_context(0);
    emscripten_start_wasm_audio_worklet_thread_async(audioContext, audioThreadStack, sizeof(audioThreadStack),
                                                     &onAudioThreadInitialized, this);
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