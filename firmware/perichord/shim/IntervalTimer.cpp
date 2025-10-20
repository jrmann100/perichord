#include "IntervalTimer.h"
#include <vector>

static constexpr int MAX_INTERVALS = 4;
static IntervalTimer *active_timers[MAX_INTERVALS] = {nullptr, nullptr, nullptr, nullptr};

// must match friend declaration — no extern "C"
void interval_wrapper(void *arg)
{
    int idx = static_cast<int>(reinterpret_cast<intptr_t>(arg));
    if (idx >= 0 && idx < MAX_INTERVALS && active_timers[idx] && active_timers[idx]->func)
        active_timers[idx]->func();
}

bool IntervalTimer::beginImpl(callback_t funct, double period_us)
{
    if (!funct || period_us <= 0.0)
        return false;

    func = funct;

    // find free slot
    int free_idx = -1;
    for (int i = 0; i < MAX_INTERVALS; i++)
    {
        if (!active_timers[i])
        {
            free_idx = i;
            break;
        }
    }
    if (free_idx < 0)
        return false;

    active_timers[free_idx] = this;

#ifdef __EMSCRIPTEN__
    double ms = period_us / 1000.0;
    interval_id = emscripten_set_interval(interval_wrapper, (int)ms, (void *)(intptr_t)free_idx);
#else
    interval_id = 0; // no-op for native builds
#endif

    return true;
}

void IntervalTimer::updateImpl(double period_us)
{
#ifdef __EMSCRIPTEN__
    if (interval_id >= 0)
    {
        // find current slot index
        int idx = -1;
        for (int i = 0; i < MAX_INTERVALS; i++)
        {
            if (active_timers[i] == this)
            {
                idx = i;
                break;
            }
        }
        if (idx < 0)
            return;

        emscripten_clear_interval(interval_id);
        double ms = period_us / 1000.0;
        interval_id = emscripten_set_interval(interval_wrapper, (int)ms, (void *)(intptr_t)idx);
    }
#endif
}

void IntervalTimer::end()
{
#ifdef __EMSCRIPTEN__
    if (interval_id >= 0)
    {
        emscripten_clear_interval(interval_id);
        interval_id = -1;
    }
#endif

    for (int i = 0; i < MAX_INTERVALS; i++)
    {
        if (active_timers[i] == this)
            active_timers[i] = nullptr;
    }

    func = nullptr;
}
