#include <Audio.h>

// Create the audio components
AudioSynthNoiseWhite noise1;
AudioOutputI2S i2s1;

// Connect white noise to both left and right channels
AudioConnection patchCord1(noise1, 0, i2s1, 0);
AudioConnection patchCord2(noise1, 0, i2s1, 1);

void setup()
{
    // Audio library requires memory allocation
    AudioMemory(10);

    // Set amplitude (0.0 to 1.0)
    noise1.amplitude(0.5);
}

void loop()
{
    // Nothing needed here - audio plays continuously
}