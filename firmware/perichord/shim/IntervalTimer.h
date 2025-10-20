/* WASM / Emscripten IntervalTimer Shim */

#ifdef __cplusplus
#ifndef __INTERVALTIMER_SHIM_H__
#define __INTERVALTIMER_SHIM_H__

#include <stddef.h>
#include <stdint.h>

#ifdef __EMSCRIPTEN__
#include <emscripten/html5.h>
#else
inline int emscripten_set_interval(void (*)(void *), int, void *) { return 0; }
inline void emscripten_clear_interval(int) {}
#endif

class IntervalTimer
{
public:
    using callback_t = void (*)(void);

    constexpr IntervalTimer() = default;
    ~IntervalTimer() { end(); }

    template <typename period_t>
    bool begin(callback_t funct, period_t period)
    {
        return beginImpl(funct, static_cast<double>(period));
    }

    void end();
    template <typename period_t>
    void update(period_t period)
    {
        updateImpl(static_cast<double>(period));
    }

    void priority(uint8_t) {}

private:
    bool beginImpl(callback_t funct, double period_us);
    void updateImpl(double period_us);

    int interval_id = -1;
    callback_t func = nullptr;

    // allow wrapper access to private members
    friend void interval_wrapper(void *arg);
};

#endif // __INTERVALTIMER_SHIM_H__
#endif // __cplusplus
