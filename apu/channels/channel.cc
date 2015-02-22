#include "channel.h"

void Channel::SetEnable(bool enabled) {        
    enabled_ = enabled;
    if (!enabled_)
        length_counter_.set_value(0);
}

void Channel::EnvelopeClock() {  // called in quarter frames
    if (envelope_counter_.reload_flag())
        envelope_counter_.Clock();
    envelope_divider_.Clock();
}

uint8_t Channel::GetCurrent() {
    return 0;
}

void Channel::CounterCallback(Counter *c) {
    if (c == &envelope_divider_)
        envelope_counter_.Clock();
    else
        ChannelSpecificCallback(c);
}

void Channel::CounterReloadCallback(Counter *c) {}
