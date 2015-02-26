#include "triangle.h"

void Triangle::SetByte(uint16_t addr, uint8_t b) {
    if (addr == 0x4008)
        Set4008(b);
    if (addr == 0x400A)
        Set400A(b);
    if (addr == 0x400B)
        Set400B(b);
}

void Triangle::Set4008(uint8_t b) {  // length counter disable, linear counter load
    length_counter_.set_halt(b >> 7);
    linear_counter_control_flag_ = b >> 7;
    if(!linear_counter_control_flag_)
        linear_counter_.clear_reload_flag_next_clock();
    linear_counter_.set_reload(b & 0x7F);
}

void Triangle::Set400A(uint8_t b) {  // period low
    timer_counter_.set_reload((timer_counter_.reload() & 0x700) | b);
}

void Triangle::Set400B(uint8_t b) {  // length counter bitload, period high
    timer_counter_.set_reload((timer_counter_.reload() & 0xFF) | ((b & 7) << 8));
    if (enabled_)
        length_counter_.set_value(kLengthCounterTable[b >> 3]);
    linear_counter_.enable_reload_flag(linear_counter_control_flag_);
}

uint8_t Triangle::GetCurrent() {
    return kSequence[sequencer_counter_.value()];
}

void Triangle::SequencerClock() {
    // triangle silences at length counter == 1
    if (linear_counter_.value() > 0 and length_counter_.value() > 1) 
        sequencer_counter_.Clock();
}

void Triangle::ChannelSpecificCounterCallback(Counter *c) {
    if (c == &timer_counter_)
        SequencerClock();
}
