#include "pulse.h"

void Pulse::SetEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_)
        length_ = 0;
}

void Pulse::SetByte(uint16_t addr, uint8_t b) {
    switch(addr) {
    case 0x4000:
    case 0x4004:  // duty, length disable, env disable, volume
        duty_ = b >> 6;
        control_flag_ = b >> 5 & 1;
        constant_volume_flag_ = b >> 4 & 1;
        constant_volume_ = b & 15;
        envelope_.reload = b & 15;
        break;
    case 0x4001:
    case 0x4005:  // sweep: enabled, period, mode, shift
        sweep_enabled_ = b >> 7;
        sweep_mode_ = -2*(b >> 3 & 1)+1;
        sweep_.reload = b >> 4 & 7;
        sweep_shift_ = b & 7;
        sweep_reload_flag_ = true;
        break;
    case 0x4002:
    case 0x4006:  // period low
        timer_.reload = (timer_.reload & 0x700) | b;
        break;
    case 0x4003:
    case 0x4007:  // length bitload, period high
        timer_.reload = (timer_.reload & 0xFF) | (b & 7) << 8;
        if (enabled_)
            length_ = kLengthTable[b >> 3];
        sequencer_ = 0;
        envelope_start_flag_ = true;
        break;
    }
}

void Pulse::TimerClock() {
    if (!timer_.counter) {
        timer_.counter = timer_.reload;
        sequencer_++;
        if (sequencer_ == kSequenceLength)
            sequencer_ = 0;
    } else timer_.counter--;
}

void Pulse::EnvelopeClock() {
    if (!envelope_start_flag_)
        if (!envelope_.counter) {
            if (envelope_volume_) envelope_volume_--;
            else if (control_flag_) envelope_volume_ = 15;
            envelope_.counter = envelope_.reload;
        }
        else envelope_.counter--;
    else {
        envelope_volume_ = 15;
        envelope_.counter = envelope_.reload;
        envelope_start_flag_ = false;
    }
}

void Pulse::LengthClock() {
    if (!control_flag_ && length_)
        length_--;
}

void Pulse::SweepClock() {
    if (!sweep_reload_flag_ && sweep_.counter)
        sweep_.counter--;
    if (sweep_enabled_ && !sweep_.counter) {
        sweep_.counter = sweep_.reload;
        int target = SweepGetTarget();
        if (target <= 0x7FF && sweep_shift_)
            timer_.reload = target;
    } 
    if (sweep_reload_flag_) {
        sweep_.counter = sweep_.reload;
        sweep_reload_flag_ = false;
    }
}

uint8_t Pulse::GetCurrent() {
    uint8_t source = constant_volume_flag_ ? constant_volume_ : envelope_volume_;
    return source * kSequences[duty_][sequencer_] *
        (SweepGetTarget() <= 0x7FF) *
        (timer_.reload >= 8) * (length_ > 0);
}

int Pulse::SweepGetTarget() { 
    int16_t shift_result = timer_.reload >> sweep_shift_;
    shift_result *= sweep_mode_;
    if(sweep_mode_ == -1 && is_pulse_1_) shift_result--;
    return timer_.reload + shift_result;
}
