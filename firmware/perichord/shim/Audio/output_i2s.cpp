#include "output_i2s.h"
#include <cstring>

void AudioOutputI2S::update(void)
{
    audio_block_t *blockL = receiveReadOnly(0); // Left channel
    audio_block_t *blockR = receiveReadOnly(1); // Right channel

    // Copy audio data to output buffers
    if (blockL)
    {
        memcpy(outputBuffer[0], blockL->data, sizeof(int16_t) * AUDIO_BLOCK_SAMPLES);
        release(blockL);
    }
    else
    {
        // No input, output silence
        memset(outputBuffer[0], 0, sizeof(int16_t) * AUDIO_BLOCK_SAMPLES);
    }

    if (blockR)
    {
        memcpy(outputBuffer[1], blockR->data, sizeof(int16_t) * AUDIO_BLOCK_SAMPLES);
        release(blockR);
    }
    else
    {
        // No input, output silence
        memset(outputBuffer[1], 0, sizeof(int16_t) * AUDIO_BLOCK_SAMPLES);
    }
}