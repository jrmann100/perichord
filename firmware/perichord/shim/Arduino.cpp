#include "Arduino.h"
#include "def.h"
#include <map>

extern "C"
{
    EM_JS(void, analogWrite, (uint8_t pin, uint8_t value), {
        Module.dispatchEvent(new CustomEvent('analogwrite', {detail : {pin : pin, value : value}}));
    });
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    // TODO
}

std::map<uint8_t, uint8_t> pinMap;

void pinMode(uint8_t pin, uint8_t mode)
{
    if (mode == INPUT)
    {
        pinMap[pin] = LOW;
        EM_ASM_({ Module.dispatchEvent(new CustomEvent('pinmode', {detail : {pin : $0, mode : $1, ptr : $2}})); }, pin, mode, &pinMap[pin]);
    }
}

uint8_t digitalRead(uint8_t pin)
{
    return pinMap[pin];
}