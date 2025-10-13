#ifndef EFFECT_PLATERVBSTEREO_H
#define EFFECT_PLATERVBSTEREO_H

#include "Audio.h"

// Plate reverb effect shim
// The actual implementation will be in JavaScript using convolution or feedback delay network
class AudioEffectPlateReverb : public AudioStream
{
public:
    AudioEffectPlateReverb() : AudioStream(1, 2)
    {
        size_ = 1.0f;
        lowpass_ = 0.3f;
        hidamp_ = 0.2f;
        lodamp_ = 0.1f;
    }

    void size(float n)
    {
        if (n < 0.0f)
            n = 0.0f;
        else if (n > 1.0f)
            n = 1.0f;
        size_ = n;
        update_js();
    }

    void lowpass(float n)
    {
        if (n < 0.0f)
            n = 0.0f;
        else if (n > 1.0f)
            n = 1.0f;
        lowpass_ = n;
        update_js();
    }

    void hidamp(float n)
    {
        if (n < 0.0f)
            n = 0.0f;
        else if (n > 1.0f)
            n = 1.0f;
        hidamp_ = n;
        update_js();
    }

    void lodamp(float n)
    {
        if (n < 0.0f)
            n = 0.0f;
        else if (n > 1.0f)
            n = 1.0f;
        lodamp_ = n;
        update_js();
    }

private:
    float size_;
    float lowpass_;
    float hidamp_;
    float lodamp_;

    void update_js()
    {
        js_audio_create_object(object_id, "AudioEffectPlateReverb");
        js_audio_set_param(object_id, "size", size_);
        js_audio_set_param(object_id, "lowpass", lowpass_);
        js_audio_set_param(object_id, "hidamp", hidamp_);
        js_audio_set_param(object_id, "lodamp", lodamp_);
    }
};

#endif // EFFECT_PLATERVBSTEREO_H
