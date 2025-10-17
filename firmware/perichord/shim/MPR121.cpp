#include <MPR121.h>

MPR121::MPR121()
{
    // Hardware version initialises internal bookkeeping and points the driver
    // at the chosen I2C bus. The shim keeps the constructor so global objects
    // in the firmware can be instantiated without touching the wire layers.
}

void MPR121::setBaselineTracking(DeviceAddress device_address,
                                 BaselineTracking baseline_tracking)
{
    // Teensy code writes the requested baseline tracking mode into the device
    // registers over I2C. The shim ignores the request but keeps the signature
    // so higher-level setup sequences still run.
    (void)device_address;
    (void)baseline_tracking;
}

void MPR121::setSamplePeriod(DeviceAddress device_address,
                             SamplePeriod sample_period)
{
    // This would normally change how frequently the controller samples the
    // electrodes. We do not emulate the sensor, so the parameters are unused.
    (void)device_address;
    (void)sample_period;
}
