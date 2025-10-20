#include <button_matrix.h>
#include <debouncer.h>
#include <emscripten.h>

void button_matrix::setup()
{
    EM_ASM({ Module.dispatchEvent(new CustomEvent('chord-setup', {detail : $0})); }, &this->data_array);
}

void button_matrix::update(debouncer (&data_array)[22])
{
    for (int i = 0; i < 22; i++)
    {
        data_array[i].set(this->data_array[i]);
    }
}
