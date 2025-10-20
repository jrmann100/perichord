#include <harp.h>

void harp::setup()
{
    EM_ASM({ Module.dispatchEvent(new CustomEvent('harp-setup', {detail : $0})); }, &this->data_array);
}

void harp::update(debouncer (&data_array)[12])
{
    for (int i = 0; i < 12; i++)
    {
        data_array[i].set(this->data_array[i]);
    }
}
