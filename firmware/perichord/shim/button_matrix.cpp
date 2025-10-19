#include <button_matrix.h>
#include <debouncer.h>
#include <emscripten.h>

button_matrix::button_matrix(uint8_t d_in_pin,
                             uint8_t storage_clock_pin,
                             uint8_t shift_clock_pin,
                             uint8_t read_pin_1_pin,
                             uint8_t read_pin_2_pin,
                             uint8_t read_pin_3_pin)
{
}

void button_matrix::setup()
{
    EM_ASM({ Module.dispatchEvent(new CustomEvent('buttonmatrix-setup', {detail : $0})); }, &this->data_array);
}

void button_matrix::update(debouncer (&data_array)[22])
{
    // In the shim we do not have shift registers, so we simulate the button
    // matrix by directly reading the pin states into the debouncers.
    for (int i = 0; i < 22; i++)
    {
        data_array[i].set(this->data_array[i]);
    }
}
