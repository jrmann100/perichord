#ifndef ARDUINO_WASM_SHIM_H
#define ARDUINO_WASM_SHIM_H

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <emscripten.h>
#include <stdint.h>
#include <string>
#include <time.h>

class String : public std::string
{
public:
    // Default and copy/move
    String() : std::string() {}
    String(const String &other) : std::string(other) {}
    String(String &&other) noexcept : std::string(std::move(other)) {}
    String(const std::string &s) : std::string(s) {}
    String(std::string &&s) : std::string(std::move(s)) {}

    // C-string and character constructors
    String(const char *s) : std::string(s ? s : "") {}
    String(char c) : std::string(1, c) {}
    String(unsigned char c) : std::string(1, static_cast<char>(c)) {}

    // Numeric constructors
    String(int value) : std::string(std::to_string(value)) {}
    String(unsigned int value) : std::string(std::to_string(value)) {}
    String(long value) : std::string(std::to_string(value)) {}
    String(unsigned long value) : std::string(std::to_string(value)) {}
    String(long long value) : std::string(std::to_string(value)) {}
    String(unsigned long long value) : std::string(std::to_string(value)) {}
    String(float value, int precision = 6) : std::string([](float v, int p)
                                                         { char b[64]; snprintf(b, sizeof(b), "%.*f", p, v); return std::string(b); }(value, precision)) {}
    String(double value, int precision = 6) : std::string([](double v, int p)
                                                          { char b[64]; snprintf(b, sizeof(b), "%.*f", p, v); return std::string(b); }(value, precision)) {}

    // Simple Arduino-like methods
    int length() const { return static_cast<int>(size()); }
    int toInt() const { return std::atoi(c_str()); }
    long toLong() const { return std::atol(c_str()); }
    float toFloat() const { return std::strtof(c_str(), nullptr); }
    double toDouble() const { return std::strtod(c_str(), nullptr); }

    // Concatenation helpers
    String &operator+=(const String &rhs)
    {
        std::string::operator+=(rhs);
        return *this;
    }
    String &operator+=(const char *rhs)
    {
        std::string::operator+=(rhs);
        return *this;
    }
    String &operator+=(char rhs)
    {
        std::string::operator+=(rhs);
        return *this;
    }

    friend String operator+(const String &a, const String &b) { return String(std::string(a) + std::string(b)); }
    friend String operator+(const String &a, const char *b) { return String(std::string(a) + std::string(b ? b : "")); }
    friend String operator+(const char *a, const String &b) { return String(std::string(a ? a : "") + std::string(b)); }
    friend String operator+(const String &a, char b)
    {
        std::string tmp(a);
        tmp.push_back(b);
        return String(std::move(tmp));
    }
    friend String operator+(char a, const String &b)
    {
        std::string tmp(1, a);
        tmp += b;
        return String(std::move(tmp));
    }
};

#define DMAMEM

#define __disable_irq() ((void)0)
#define __enable_irq() ((void)0)

class IntervalTimer
{
public:
    void begin(void (*f)(), unsigned long us) {}
    void end() {}
};

template <typename T>
inline T constrain(T amt, T low, T high)
{
    if (amt < low)
        return low;
    if (amt > high)
        return high;
    return amt;
}

inline float map(float x, float in_min, float in_max, float out_min, float out_max)
{
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

#define PI 3.1415926535897932384626433832795

// Board-specific CPU frequency definition
#ifndef F_CPU_ACTUAL
#define F_CPU_ACTUAL 600000000 // 600 MHz, typical for Teensy 4.x
#endif

typedef uint32_t u_int32_t;
typedef uint16_t u_int16_t;
typedef uint8_t u_int8_t;
typedef uint8_t byte;
typedef unsigned long elapsedMillis;
typedef unsigned long elapsedMicros;

class SerialShim
{
public:
    void println(const char *) {}
    void println(unsigned char) {}
    void print(const char *) {}
    void print(int) {}
    void begin(unsigned long) {}
};

static SerialShim Serial;

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1

void analogWrite(uint8_t pin, uint8_t value) {}
void digitalWrite(uint8_t pin, uint8_t value) {}

class usb_midi_class
{
public:
    // Message type names for compatibility with Arduino MIDI library 4.3.1
    enum MidiType
    {
        InvalidType = 0x00,
        NoteOff = 0x80,
        NoteOn = 0x90,
        AfterTouchPoly = 0xA0,
        ControlChange = 0xB0,
        ProgramChange = 0xC0,
        AfterTouchChannel = 0xD0,
        PitchBend = 0xE0,
        SystemExclusive = 0xF0,
        TimeCodeQuarterFrame = 0xF1,
        SongPosition = 0xF2,
        SongSelect = 0xF3,
        TuneRequest = 0xF6,
        Clock = 0xF8,
        Start = 0xFA,
        Continue = 0xFB,
        Stop = 0xFC,
        ActiveSensing = 0xFE,
        SystemReset = 0xFF
    };

    // Noop implementations for required functions
    void sendSysEx(uint32_t length, const uint8_t *data, bool hasTerm = false, uint8_t cable = 0) {}
    uint8_t getType(void) { return 0; }
    uint16_t getSysExArrayLength(void) { return 0; }
    uint8_t *getSysExArray(void) { return nullptr; }
    void sendNoteOff(uint8_t note, uint8_t velocity, uint8_t channel, uint8_t cable = 0) {}
    void sendNoteOn(uint8_t note, uint8_t velocity, uint8_t channel, uint8_t cable = 0) {}
};

static usb_midi_class usbMIDI;

#endif // ARDUINO_WASM_SHIM_H
