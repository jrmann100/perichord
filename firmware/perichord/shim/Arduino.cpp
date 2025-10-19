#include "Arduino.h"
#include "def.h"
#include <map>

void analogWrite(uint8_t pin, uint8_t value)
{
    EM_ASM_({ Module.dispatchEvent(new CustomEvent('analogwrite', {detail : {pin : $0, value : $1}})); }, pin, value);
}

void digitalWrite(uint8_t pin, uint8_t value)
{
    // TODO
}

uint8_t pinValues[34];

void pinMode(uint8_t pin, uint8_t mode)
{
    if (mode == INPUT)
    {
        pinValues[pin] = LOW;
        EM_ASM_({ Module.dispatchEvent(new CustomEvent('pinmode', {detail : {pin : $0, mode : $1, ptr : $2}})); }, pin, mode, &pinValues[pin]);
    }
}

uint8_t digitalRead(uint8_t pin)
{
    return pinValues[pin];
}