#ifndef MPR121_H
#define MPR121_H
#include <Arduino.h>
#include <Wire.h>

class MPR121
{
public:
    MPR121();

    enum DeviceAddress
    {
        ADDRESS_5A = 0x5A,
        ADDRESS_5B = 0x5B,
        ADDRESS_5C = 0x5C,
        ADDRESS_5D = 0x5D,
    };
    // enum
    // {
    //     PHYSICAL_CHANNELS_PER_DEVICE = 12
    // };
    // enum
    // {
    //     CHANNELS_PER_DEVICE = 13
    // };
    // enum
    // {
    //     DEVICE_COUNT_MAX = 4
    // };

    // // Convenience method when using a single device
    // bool setupSingleDevice(TwoWire &wire = Wire,
    //                        DeviceAddress device_address = ADDRESS_5A,
    //                        bool fast_mode = false);

    // // Methods for using a single device or multiple devices
    // bool communicating(DeviceAddress device_address);

    // enum ProximityMode
    // {
    //     PROXIMITY_MODE_DISABLED = 0b00,
    //     COMBINE_CHANNELS_0_TO_1 = 0b01,
    //     COMBINE_CHANNELS_0_TO_3 = 0b10,
    //     COMBINE_CHANNELS_0_TO_11 = 0b11,
    // };
    // void startChannels(uint8_t physical_channel_count,
    //                    ProximityMode proximity_mode = PROXIMITY_MODE_DISABLED);
    // void startAllChannels(ProximityMode proximity_mode = PROXIMITY_MODE_DISABLED);
    // void stopAllChannels();

    // uint8_t getChannelCount();
    // uint8_t getRunningChannelCount();

    // // sets touch and release thresholds either for all channels, or
    // // for a specfic channel - higher values = less sensitive
    // void setChannelThresholds(uint8_t channel,
    //                           uint8_t touch_threshold,
    //                           uint8_t release_threshold);
    // void setAllChannelsThresholds(uint8_t touch_threshold,
    //                               uint8_t release_threshold);

    // bool channelTouched(uint8_t channel);

    // uint16_t getChannelFilteredData(uint8_t channel);
    // uint16_t getChannelBaselineData(uint8_t channel);

    // // Methods for using multiple devices
    // // Take care when using fast_mode with non-MPR121 devices
    // void setWire(TwoWire &wire = Wire,
    //              bool fast_mode = false);

    // void addDevice(DeviceAddress device_address);
    // bool setupDevice(DeviceAddress device_address);
    // bool setupAllDevices();

    // void startChannels(DeviceAddress device_address,
    //                    uint8_t physical_channel_count,
    //                    ProximityMode proximity_mode = PROXIMITY_MODE_DISABLED);
    // void startChannelsAllDevices(uint8_t physical_channel_count,
    //                              ProximityMode proximity_mode = PROXIMITY_MODE_DISABLED);
    // void startAllChannels(DeviceAddress device_address,
    //                       ProximityMode proximity_mode = PROXIMITY_MODE_DISABLED);
    // void stopAllChannels(DeviceAddress device_address);

    // uint8_t getDeviceCount();
    // uint8_t getDeviceChannelCount();
    // uint8_t getRunningChannelCount(DeviceAddress device_address);

    // void setDeviceChannelThresholds(DeviceAddress device_address,
    //                                 uint8_t device_channel,
    //                                 uint8_t touch_threshold,
    //                                 uint8_t release_threshold);
    // void setAllDeviceChannelsThresholds(DeviceAddress device_address,
    //                                     uint8_t touch_threshold,
    //                                     uint8_t release_threshold);

    // uint16_t getTouchStatus(DeviceAddress device_address);
    // bool overCurrentDetected(uint16_t touch_status);
    // bool anyTouched(uint16_t touch_status);
    // uint8_t getTouchCount(uint16_t touch_status);
    // bool deviceChannelTouched(uint16_t touch_status,
    //                           uint8_t device_channel);

    // uint16_t getOutOfRangeStatus(DeviceAddress device_address);
    // bool deviceChannelOutOfRange(uint16_t out_of_range_status,
    //                              uint8_t device_channel);
    // bool autoConfigFail(uint16_t out_of_range_status);
    // bool autoReconfigFail(uint16_t out_of_range_status);

    // uint16_t getDeviceChannelFilteredData(DeviceAddress device_address,
    //                                       uint8_t device_channel);
    // uint16_t getDeviceChannelBaselineData(DeviceAddress device_address,
    //                                       uint8_t device_channel);

    enum BaselineTracking
    {
        BASELINE_TRACKING_INIT_0 = 0x00, // default
        BASELINE_TRACKING_DISABLED = 0x01,
        BASELINE_TRACKING_INIT_5BIT = 0x02,
        BASELINE_TRACKING_INIT_10BIT = 0x03,
    };
    void setBaselineTracking(DeviceAddress device_address,
                             BaselineTracking baseline_tracking);
    // void setDebounce(DeviceAddress device_address,
    //                  uint8_t touch_debounce,
    //                  uint8_t release_debounce);

    // const static uint8_t CHARGE_DISCHARGE_CURRENT_MIN = 1;
    // const static uint8_t CHARGE_DISCHARGE_CURRENT_MAX = 63;
    // void setChargeDischargeCurrent(DeviceAddress device_address,
    //                                uint8_t charge_discharge_current);
    // void setDeviceChannelChargeDischargeCurrent(DeviceAddress device_address,
    //                                             uint8_t device_channel,
    //                                             uint8_t charge_discharge_current);
    enum ChargeDischargeTime
    {
        CHARGE_DISCHARGE_TIME_DISABLED = 0x00,
        CHARGE_DISCHARGE_TIME_HALF_US = 0x01, // default
        CHARGE_DISCHARGE_TIME_1US = 0x02,
        CHARGE_DISCHARGE_TIME_2US = 0x03,
        CHARGE_DISCHARGE_TIME_4US = 0x04,
        CHARGE_DISCHARGE_TIME_8US = 0x05,
        CHARGE_DISCHARGE_TIME_16US = 0x06,
        CHARGE_DISCHARGE_TIME_32US = 0x07
    };
    // void setChargeDischargeTime(DeviceAddress device_address,
    //                             ChargeDischargeTime charge_discharge_time);
    // void setDeviceChannelChargeDischargeTime(DeviceAddress device_address,
    //                                          uint8_t device_channel,
    //                                          ChargeDischargeTime charge_discharge_time);

    enum FirstFilterIterations
    {
        FIRST_FILTER_ITERATIONS_6 = 0x00, // default
        FIRST_FILTER_ITERATIONS_10 = 0x01,
        FIRST_FILTER_ITERATIONS_18 = 0x02,
        FIRST_FILTER_ITERATIONS_34 = 0x03,
    };
    // void setFirstFilterIterations(DeviceAddress device_address,
    //                               FirstFilterIterations first_filter_iterations);
    enum SecondFilterIterations
    {
        SECOND_FILTER_ITERATIONS_4 = 0x00, // default
        SECOND_FILTER_ITERATIONS_6 = 0x01,
        SECOND_FILTER_ITERATIONS_10 = 0x02,
        SECOND_FILTER_ITERATIONS_18 = 0x03,
    };
    // void setSecondFilterIterations(DeviceAddress device_address,
    //                                SecondFilterIterations second_filter_iterations);

    // Sets the sample period of the MPR121 - the time between capacitive
    // readings. Higher values consume less power, but are less responsive.
    // sample intervals
    enum SamplePeriod
    {
        SAMPLE_PERIOD_1MS = 0x00,
        SAMPLE_PERIOD_2MS = 0x01,
        SAMPLE_PERIOD_4MS = 0x02,
        SAMPLE_PERIOD_8MS = 0x03,
        SAMPLE_PERIOD_16MS = 0x04, // default
        SAMPLE_PERIOD_32MS = 0x05,
        SAMPLE_PERIOD_64MS = 0x06,
        SAMPLE_PERIOD_128MS = 0x07
    };
    void setSamplePeriod(DeviceAddress device_address,
                         SamplePeriod sample_period);
};

#endif