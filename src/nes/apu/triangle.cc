#include "triangle.h"

void nes_apu::Triangle::SetEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_)
        length_ = 0;
}

void nes_apu::Triangle::SetByte(uint16_t addr, uint8_t b) {
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
            length_ = kLengthTable[b >> 3] - 1;
        linear_reload_flag_ = true;
        break;
    }
}

void nes_apu::Triangle::TimerClock() {
    if (!timer_.counter) {
        timer_.counter = timer_.reload;
        if (length_ && linear_.counter) {
            sequencer_++;
            if (sequencer_ == kSequenceLength)
                sequencer_ = 0;
        }
    } else timer_.counter--;
}

void nes_apu::Triangle::LengthClock() {
    if (!control_flag_ && length_)
        length_--;
}

void nes_apu::Triangle::LinearClock() {
    if (linear_.counter)
        linear_.counter--;
    if (linear_reload_flag_)
        linear_.counter = linear_.reload;
    if (!control_flag_)
        linear_reload_flag_ = false;
}
