#include <harp.h>

harp::harp()
{
    // The live instrument selects between AT42QT2120 or MPR121 capacitive touch
    // controllers here. The shim does not instantiate either device; the class
    // simply exists so the firmware can build.
}

void harp::setup()
{
    // In hardware this configures the chosen touch controller and prepares the
    // I2C bus. There is nothing to initialise in the browser shim, so the call
    // becomes a no-op.
}

void harp::recalibrate()
{
    // The firmware runs the controller's calibration routine to establish
    // baseline touch values. Calibration is unnecessary in the shim, but we
    // retain the entry point to match the original control flow.
}

void harp::update(debouncer (&data_array)[12])
{
    // Real hardware reads the touch sensor status registers, maps the values to
    // string indices, and updates the debouncers. For the shim we simply clear
    // the array so the rest of the code believes no strings are active.
    for (int i = 0; i < 12; ++i)
    {
        data_array[i].set(false);
    }
}
