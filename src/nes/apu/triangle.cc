#include "triangle.h"
#include <stdio.h>

void Triangle::SetEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_)
        length_ = 0;
}

void Triangle::SetByte(uint16_t addr, uint8_t b) {
    switch(addr) {
    case 0x4008:  // length disable / linear control flag, linear load
        control_flag_ = b >> 7;
        linear_.reload = b & 0x7F;
        break;
    case 0x400A:  // period low
        timer_.reload = (timer_.reload & 0x700) | b;
        break;
    case 0x400B:  // length counter bitload, period high
        timer_.reload = (timer_.reload & 0xFF) | (b & 7) << 8;
        if (enabled_)
            length_ = kLengthTable[b >> 3] + 1;  // TODO: Why +1 needed?
        linear_reload_flag_ = true;
        break;
    }
}

void Triangle::TimerClock() {
    if (timer_.reload > 1) {
        if (!timer_.counter) {
            timer_.counter = timer_.reload;
            if (length_ && linear_.counter) {
                sequencer_++;
                if (sequencer_ == kSequenceLength)
                    sequencer_ = 0;
            }
        } else timer_.counter--;
    }
}

void Triangle::LengthClock() {
    if (!control_flag_ && length_)
        length_--;
}

void Triangle::LinearClock() {
    if (linear_.counter)
        linear_.counter--;
    if (linear_reload_flag_)
        linear_.counter = linear_.reload;
    if (!control_flag_)
        linear_reload_flag_ = false;
}
