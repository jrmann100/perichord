#pragma once

#include <Audio.h>
#include <cstdint>
#include <emscripten/bind.h>
#include <emscripten/val.h>
#include <emscripten/webaudio.h>

#define PERICHORD_AUDIO_PROCESSOR_NAME "perichord-processor"

class PerichordAudioWorklet
{
public:
    PerichordAudioWorklet(emscripten::val readyCallback);
    EMSCRIPTEN_WEBAUDIO_T audioContext;

private:
    uint8_t audioThreadStack[4096];
    emscripten::val readyCallback;

    static void onAudioThreadInitialized(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData);
    static void onAudioWorkletProcessorCreated(EMSCRIPTEN_WEBAUDIO_T audioContext, bool success, void *userData);
};

EMSCRIPTEN_BINDINGS(perichord_bindings)
{
    emscripten::class_<PerichordAudioWorklet>("PerichordAudioWorklet")
        .constructor<emscripten::val>()
        .property("audioContext", &PerichordAudioWorklet::audioContext);
}