#include "pulse.h"

void Pulse::Set4000(uint8_t b) {  // duty, len counter disable, env disable, volume
    duty_ = b >> 6;
    length_counter_.set_halt(b >> 5 & 1);
    envelope_counter_.set_loop(b >> 5 & 1);
    envelope_counter_.set_halt(b >> 4 & 1);
    envelope_divider_.set_reload(b & 15);
    volume_ = b & 15;
}

void Pulse::Set4001(uint8_t b) {  // sweep: enabled, div period, neg flag, shift count
    sweep_counter_.set_halt(b >> 7);
    sweep_counter_.set_reload(b >> 4 & 7);
    sweep_counter_.enable_reload_flag();
    sweep_positive_ = -2*(b >> 3 & 1) + 1;  // -1 if negate flag, 1 if not
    sweep_shift_ = b & 7;
}

void Pulse::Set4002(uint8_t b) {  // period low
    timer_counter_.set_reload((timer_counter_.reload() & 1792) | b);
}

void Pulse::Set4003(uint8_t b) {  // length counter bitload, period high
    timer_counter_.set_reload((timer_counter_.reload() & 255) | ((b & 7) << 8));
    if (enabled_)
        length_counter_.set_value(kLengthCounterTable[b >> 3]);
    sequencer_counter_.set_value(0);
    envelope_counter_.enable_reload_flag();
    envelope_divider_.enable_reload_flag();
}

uint8_t Pulse::GetCurrent() {
    if (timer_counter_.reload() >= 8 and
        kSequences[duty_][sequencer_counter_.value()] and
        SweepGetTarget() <= 2047 and length_counter_.value()) {
        if (envelope_counter_.halt())
            return volume_;
        else
            return envelope_counter_.value();
    } else
        return 0;
}

int Pulse::SweepGetTarget() { 
    int16_t shift_result = timer_counter_.reload() >> sweep_shift_;
    shift_result *= sweep_positive_;
    if(sweep_positive_ < 0 and is_pulse_1_) shift_result--;
    return timer_counter_.reload() + shift_result;
}

void Pulse::SweepAdjustPeriod() {
    if (sweep_shift_)
        timer_counter_.set_reload(SweepGetTarget());
}

void Pulse::CounterReloadCallback(Counter *c) {
    if (c == &sweep_counter_)
        SweepAdjustPeriod();
}

void Pulse::ChannelSpecificCounterCallback(Counter *c) {
    if (c == &sweep_counter_)
        SweepAdjustPeriod();
    else if (c == &timer_counter_)
        SequencerClock();
}
