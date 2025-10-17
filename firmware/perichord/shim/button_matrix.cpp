#include <button_matrix.h>
#include <debouncer.h>

button_matrix::button_matrix(uint8_t d_in_pin,
                             uint8_t storage_clock_pin,
                             uint8_t shift_clock_pin,
                             uint8_t read_pin_1_pin,
                             uint8_t read_pin_2_pin,
                             uint8_t read_pin_3_pin)
    : d_in(d_in_pin),
      storage_clock(storage_clock_pin),
      shift_clock(shift_clock_pin),
      read_pin_1(read_pin_1_pin),
      read_pin_2(read_pin_2_pin),
      read_pin_3(read_pin_3_pin)
{
    // The hardware version prepares GPIO control lines for the external shift
    // register and keeps track of which input pins belong to the matrix. The
    // shim only records the pin numbers so firmware can build successfully.
}

void button_matrix::setup()
{
    // Normally this configures the GPIO directions for the 74HC595 chain and
    // enables pull-ups on the input columns. In the WebAssembly build there is
    // no hardware to configure, so we simply leave the pins idle.
}

void button_matrix::write_bit(bool data)
{
    // The live firmware toggles the shift clock and data line to push a single
    // bit into the external shift register. We do not emulate the register, so
    // the stub just acknowledges the call.
    (void)data;
}

void button_matrix::write_byte(byte data)
{
    // On device this streams eight bits to the shift register so one row of
    // the button matrix can be driven low. Keeping the call makes the control
    // flow match the firmware even though no data is shifted in the shim.
    (void)data;
}

void button_matrix::update(debouncer (&data_array)[22])
{
    // The real implementation scans each column, updates the debouncers with
    // freshly read button states, and allows the rest of the firmware to react
    // to state changes. For the shim we report that no buttons are pressed so
    // higher-level logic stays idle unless the host manually injects events.
    for (int i = 0; i < 22; ++i)
    {
        data_array[i].set(false);
    }
}
