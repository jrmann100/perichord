#include <debouncer.h>

debouncer::debouncer()
    : flag(false),
      last_update(0),
      value(false)
{
    // The Teensy version tracks elapsed time between state changes so glitches
    // shorter than debounce_value are filtered out. Inside the shim we do not
    // have microsecond timing, so we default to immediate transitions.
}

void debouncer::set(bool set_value)
{
    // Hardware code waits for debounce_value microseconds before committing the
    // new state. We record the change immediately and let read_transition()
    // report it on the next poll.
    if (value != set_value)
    {
        flag = true;
        last_update = 0;
        value = set_value;
    }
}

uint8_t debouncer::read_transition()
{
    // The real implementation checks whether enough time has passed before
    // acknowledging the transition. In the shim we return a single edge the
    // first time the new state is observed so higher level code keeps working.
    if (flag)
    {
        flag = false;
        return value ? 2 : 1;
    }
    return 0;
}

bool debouncer::read_value()
{
    return value;
}
