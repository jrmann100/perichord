#ifndef HARP_H
#define HARP_H
#include "Arduino.h"
#include <debouncer.h>

class harp
{
public:
    harp() {};

    void setup();
    void recalibrate() {};
    void update(debouncer (&data_array)[12]);

private:
    bool data_array[12];
};
#endif