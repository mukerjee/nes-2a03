#include "triangle.h"

void Triangle::Set4008(uint8_t b) {  // length counter disable, linear counter load
    length_counter_.set_halt(b >> 7);
    if(! b >> 7)
        linear_counter.clear_reload_flag_next_clock();
    linear_counter_.set_reload(b & 127);
}

void Triangle::Set400A(uint8_t b) {  // period low
    timer_counter_.set_reload((timer_counter_.reload() | 255) & b);
}

void Triangle::Set400B(uint8_t b) {  // length counter bitload, period high
    if (enabled_)
        length_counter_.set_value(kLengthCounterTable[b >> 3]);
    timer_counter_.set_reload((timer_counter_.reload() | 1792) & ((b & 7) << 8));
    linear_counter_.enable_reload_flag(true);
}

void Triangle::SequencerClock() {
    if (linear_counter_counter_ > 0 and length_counter_counter_ > 0)
        sequencer_counter_.Clock();
}

void Triangle::ChannelSpecificCounterCallback(Counter *c) {
    if (c == &timer_counter_)
        SequenceClock();
}
