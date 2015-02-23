#include "channel.h"

void Channel::SetEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_)
        length_counter_.set_value(0);
}

void Channel::EnvelopeClock() {  // called in quarter frames
    if (envelope_counter_.reload_flag())
        envelope_counter_.Clock();
    envelope_divider_.Clock();
}

void Channel::CounterCallback(Counter *c) {
    if (c == &envelope_divider_)
        envelope_counter_.Clock();
    else
        ChannelSpecificCounterCallback(c);
}
