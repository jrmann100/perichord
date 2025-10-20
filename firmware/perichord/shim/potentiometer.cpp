#include <potentiometer.h>

potentiometer::potentiometer(int pot_pin_number)
    : pot_pin(pot_pin_number)
{
    // The Teensy build stores the ADC channel associated with the pot so it can
    // read voltages later. The shim only needs to remember which control this
    // instance represents.
}

void potentiometer::setup(int main_adress_param,
                          float main_range_param,
                          int alternate_adress_param,
                          float alternate_range_param,
                          int16_t *current_sysex_parameters_ptr,
                          int alternate_initial_value,
                          const std::function<void(int, int)> &apply_audio_parameter_fn,
                          int alternate_storage_adress_param)
{
    // On hardware this method initialises smoothing, thresholds, and the live
    // callback used to push values into the audio engine. Inside the shim we
    // keep the bookkeeping so the same control code can run in a browser.
    main_adress = main_adress_param;
    main_range = main_range_param;
    alternate_adress = alternate_adress_param;
    alternate_range = alternate_range_param;
    current_sysex_parameters_pointer = current_sysex_parameters_ptr;
    alternate_storage_adress = alternate_storage_adress_param;
    apply_audio_parameter = apply_audio_parameter_fn;
    alternate_value = alternate_initial_value;

    if (apply_audio_parameter)
    {
        const int main_value = current_sysex_parameters_pointer ? current_sysex_parameters_pointer[main_adress] : 0;
        apply_audio_parameter(main_adress, main_value);
        apply_audio_parameter(alternate_adress, alternate_initial_value);
    }
}

void potentiometer::set_main(int adress)
{
    // Firmware normally switches which SYSEX slot the pot controls. We mirror
    // the assignment so follow-up calls act on the updated mapping.
    main_adress = adress;
}

void potentiometer::set_main_range(float range)
{
    main_range = range;
}

void potentiometer::set_alternate(int adress)
{
    alternate_adress = adress;
}

void potentiometer::set_alternate_range(float range)
{
    alternate_range = range;
}

void potentiometer::set_alternate_default(int alternate_initial_value)
{
    alternate_value = alternate_initial_value;
}

void potentiometer::force_update()
{
    // The hardware path re-emits the most recent values so connected audio
    // objects pick up any configuration changes. We do the same with our cached
    // callback when available.
    if (!apply_audio_parameter)
    {
        return;
    }

    const int main_value = current_sysex_parameters_pointer ? current_sysex_parameters_pointer[main_adress] : 0;
    apply_audio_parameter(main_adress, main_value);
    apply_audio_parameter(alternate_adress, alternate_value);
}

bool potentiometer::update_parameter(bool alternate_flag)
{
    // Normally this reads the ADC, applies smoothing and thresholding, and then
    // dispatches any changes via apply_audio_parameter(). The shim cannot read
    // hardware, so we always report 50%
    if (apply_audio_parameter)
    {
        if (alternate_flag)
        {
            apply_audio_parameter(alternate_adress, main_range);
        }
        else
        {
            apply_audio_parameter(main_adress, main_range);
        }
        return true; // Report that a change was made
    }
    return false;
}
