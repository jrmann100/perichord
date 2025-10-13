#include "Arduino.h"
#include <emscripten.h>
#include <math.h>
#include <stdio.h>

// JavaScript function declarations (implemented in JS glue code)
extern "C"
{
    extern double js_performance_now();
    extern void js_console_log(const char *msg);
    extern void js_set_pin(int pin, int value);
    extern int js_get_pin(int pin);
    extern void js_set_timer(int timer_id, int microseconds);
    extern void js_clear_timer(int timer_id);
}

// Timing implementation using JavaScript performance.now()
static double start_time_ms = 0.0;
static bool time_initialized = false;

static void init_time()
{
    if (!time_initialized)
    {
        start_time_ms = js_performance_now();
        time_initialized = true;
    }
}

unsigned long millis()
{
    init_time();
    return (unsigned long)(js_performance_now() - start_time_ms);
}

unsigned long micros()
{
    init_time();
    return (unsigned long)((js_performance_now() - start_time_ms) * 1000.0);
}

void delay(unsigned long ms)
{
    // In WASM, we can't actually block - this is a no-op
    // Actual delays should be handled by JavaScript async/await
    (void)ms;
}

void delayMicroseconds(unsigned int us)
{
    // In WASM, we can't actually block - this is a no-op
    (void)us;
}

// Digital I/O - Virtual pin state
static uint8_t pin_modes[64] = {0};
static uint8_t pin_values[64] = {0};

void pinMode(uint8_t pin, uint8_t mode)
{
    if (pin < 64)
    {
        pin_modes[pin] = mode;
    }
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    if (pin < 64)
    {
        pin_values[pin] = val;
        js_set_pin(pin, val);
    }
}

int digitalRead(uint8_t pin)
{
    if (pin < 64)
    {
        return js_get_pin(pin);
    }
    return LOW;
}

// Analog I/O
static uint16_t analog_values[32] = {0};

int analogRead(uint8_t pin)
{
    if (pin < 32)
    {
        return js_get_pin(pin + 100); // Offset for analog pins
    }
    return 0;
}

void analogWrite(uint8_t pin, int val)
{
    if (pin < 32)
    {
        js_set_pin(pin + 100, val);
    }
}

// Serial implementation
Serial_ Serial;

void Serial_::print(const char *str)
{
    js_console_log(str);
}

void Serial_::print(int val)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", val);
    js_console_log(buf);
}

void Serial_::print(unsigned int val)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%u", val);
    js_console_log(buf);
}

void Serial_::print(long val)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%ld", val);
    js_console_log(buf);
}

void Serial_::print(unsigned long val)
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%lu", val);
    js_console_log(buf);
}

void Serial_::print(float val, int digits)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "%.*f", digits, val);
    js_console_log(buf);
}

void Serial_::println(const char *str)
{
    js_console_log(str);
}

void Serial_::println(int val)
{
    print(val);
}

void Serial_::println(unsigned int val)
{
    print(val);
}

void Serial_::println(long val)
{
    print(val);
}

void Serial_::println(unsigned long val)
{
    print(val);
}

void Serial_::println(float val, int digits)
{
    print(val, digits);
}

void Serial_::println()
{
    js_console_log("");
}

// IntervalTimer implementation
static int next_timer_id = 0;

bool IntervalTimer::begin(void (*callback)(), unsigned int microseconds)
{
    if (active_)
    {
        end();
    }
    timer_id_ = next_timer_id++;
    js_set_timer(timer_id_, microseconds);
    active_ = true;
    // Note: actual callback invocation handled by JavaScript
    return true;
}

void IntervalTimer::end()
{
    if (active_)
    {
        js_clear_timer(timer_id_);
        active_ = false;
    }
}

void IntervalTimer::update(unsigned int microseconds)
{
    if (active_)
    {
        js_set_timer(timer_id_, microseconds);
    }
}
