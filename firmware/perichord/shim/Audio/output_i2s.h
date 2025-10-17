#pragma once
#include "AudioStream.h"

class AudioOutputI2S : public AudioStream
{
public:
    AudioOutputI2S() : AudioStream(2, inputQueueArray)
    {
        // Initialize output buffer
        for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++)
        {
            outputBuffer[0][i] = 0;
            outputBuffer[1][i] = 0;
        }
    }
    virtual void update(void);

    // Get the rendered audio samples
    const int16_t *getLeftChannel() const { return outputBuffer[0]; }
    const int16_t *getRightChannel() const { return outputBuffer[1]; }

private:
    audio_block_t *inputQueueArray[2];
    int16_t outputBuffer[2][AUDIO_BLOCK_SAMPLES]; // Left and right channels
};