#ifndef ARDUINO_WASM_SHIM_H
#define ARDUINO_WASM_SHIM_H

#include <emscripten.h>
#include <stdint.h>

// Pin modes
#define INPUT 0x0
#define OUTPUT 0x1
#define INPUT_PULLUP 0x2

// Digital values
#define HIGH 0x1
#define LOW 0x0

// Timing functions
extern "C"
{
    // These will be implemented to call JS performance.now()
    EMSCRIPTEN_KEEPALIVE unsigned long millis();
    EMSCRIPTEN_KEEPALIVE unsigned long micros();
    EMSCRIPTEN_KEEPALIVE void delay(unsigned long ms);
    EMSCRIPTEN_KEEPALIVE void delayMicroseconds(unsigned int us);
}

// Digital I/O - Virtual implementations
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);

// Analog I/O - Can be mapped to Web MIDI CC or virtual controls
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int val);

// Math macros (use inline functions to avoid conflicts with std library)
inline int arduino_min(int a, int b) { return (a < b) ? a : b; }
inline float arduino_min(float a, float b) { return (a < b) ? a : b; }
inline int arduino_max(int a, int b) { return (a > b) ? a : b; }
inline float arduino_max(float a, float b) { return (a > b) ? a : b; }
inline int arduino_abs(int x) { return (x > 0) ? x : -x; }
inline float arduino_abs(float x) { return (x > 0) ? x : -x; }
inline float constrain(float amt, float low, float high)
{
    return (amt < low) ? low : ((amt > high) ? high : amt);
}
// Use std::round instead of macro
#include <cmath>
inline long arduino_round(float x) { return (long)std::round(x); }
inline float radians(float deg) { return deg * 0.017453292519943295769236907684886f; }
inline float degrees(float rad) { return rad * 57.295779513082320876798154814105f; }
inline float sq(float x) { return x * x; }

// For compatibility with Arduino code that uses min/max/abs without prefix
#ifndef min
#define min arduino_min
#endif
#ifndef max
#define max arduino_max
#endif
#ifndef abs
#define abs arduino_abs
#endif

// Constants
#define PI 3.1415926535897932384626433832795
#define HALF_PI 1.5707963267948966192313216916398
#define TWO_PI 6.283185307179586476925286766559
#define DEG_TO_RAD 0.017453292519943295769236907684886
#define RAD_TO_DEG 57.295779513082320876798154814105

// Bit manipulation
#define lowByte(w) ((uint8_t)((w) & 0xff))
#define highByte(w) ((uint8_t)((w) >> 8))
#define bitRead(value, bit) (((value) >> (bit)) & 0x01)
#define bitSet(value, bit) ((value) |= (1UL << (bit)))
#define bitClear(value, bit) ((value) &= ~(1UL << (bit)))
#define bitWrite(value, bit, bitvalue) (bitvalue ? bitSet(value, bit) : bitClear(value, bit))

// Serial interface - logs to console
class Serial_
{
public:
    void begin(unsigned long baud) { (void)baud; }
    void end() {}
    int available() { return 0; }
    int read() { return -1; }
    void print(const char *str);
    void print(int val);
    void print(unsigned int val);
    void print(long val);
    void print(unsigned long val);
    void print(float val, int digits = 2);
    void println(const char *str);
    void println(int val);
    void println(unsigned int val);
    void println(long val);
    void println(unsigned long val);
    void println(float val, int digits = 2);
    void println();
};

extern Serial_ Serial;

// Interrupt handling
class IntervalTimer
{
public:
    IntervalTimer() : active_(false) {}
    bool begin(void (*callback)(), unsigned int microseconds);
    void end();
    void update(unsigned int microseconds);
    bool isActive() { return active_; }

private:
    bool active_;
    int timer_id_;
};

// elapsedMillis and elapsedMicros - time tracking helpers
class elapsedMillis
{
public:
    elapsedMillis() { ms_ = millis(); }
    elapsedMillis(unsigned long val) { ms_ = millis() - val; }
    operator unsigned long() const { return millis() - ms_; }
    elapsedMillis &operator=(unsigned long val)
    {
        ms_ = millis() - val;
        return *this;
    }

private:
    unsigned long ms_;
};

class elapsedMicros
{
public:
    elapsedMicros() { us_ = micros(); }
    elapsedMicros(unsigned long val) { us_ = micros() - val; }
    operator unsigned long() const { return micros() - us_; }
    elapsedMicros &operator=(unsigned long val)
    {
        us_ = micros() - val;
        return *this;
    }

private:
    unsigned long us_;
};

#endif // ARDUINO_WASM_SHIM_H
