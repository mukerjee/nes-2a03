#include "noise.h"

void nes_apu::Noise::SetEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_)
        length_ = 0;
}

void nes_apu::Noise::SetByte(uint16_t addr, uint8_t b) {
    switch(addr) {
    case 0x400C:  //  length disable, env disable, volume
        control_flag_ = b >> 5 & 1;
        constant_volume_ = (b >> 4 & 1) ? (b & 15) : 0;
        envelope_.reload = b & 15;
        break;
    case 0x400E:  // mode, period
        mode_flag_ = b >> 7;
        timer_.reload = kPeriodTable[b & 15];
        break;
    case 0x400F:  // length counter bitload
        if (enabled_)
            length_ = kLengthTable[b >> 3];
        envelope_start_flag_ = true;
        break;
    }
}

void nes_apu::Noise::TimerClock() {
    if (!timer_.counter) {
        timer_.counter = timer_.reload;
        uint8_t shift_magnitude = mode_flag_ ? 6 : 1;
        uint8_t extra_bit = shift_register_ >> shift_magnitude & 1;
        uint8_t feedback = (shift_register_ & 1) ^ extra_bit;
        shift_register_ >>= 1;
        shift_register_ |= feedback << 14;       
    } else timer_.counter--;
}

void nes_apu::Noise::EnvelopeClock() {
    if (!envelope_start_flag_)
        if (!envelope_.counter) {
            if (envelope_volume_) envelope_volume_--;
            else if (control_flag_) envelope_volume_ = 15;
        }
        else envelope_.counter--;
    else {
        envelope_volume_ = 15;
        envelope_.counter = envelope_.reload;
    }
}

void nes_apu::Noise::LengthClock() {
    if (!control_flag_ && length_)
        length_--;
}

uint8_t nes_apu::Noise::GetCurrent() {
    uint8_t source = constant_volume_ ? constant_volume_ : envelope_volume_;
    return source * (shift_register_ & 1) * length_;
}
