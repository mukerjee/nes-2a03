#include "pulse.h"

void Pulse::Set4000(uint8_t b) {  // duty, len counter disable, env disable, volume
    duty_ = b >> 6;
    length_counter_.set_halt(b >> 5 & 1);
    envelope_counter.set_loop(b >> 5 & 1);
    envelope_counter_.set_halt(b >> 4 & 1);
    envelope_divider_.set_reload(b & 15);
    volume_ = b & 15;
}

void Pulse::Set4001(uint8_t b) {  // sweep: enabled, div period, neg flag, shift count
    sweep_counter_.set_halt_(b >> 7);
    sweep_counter_.set_reload(b >> 4 & 7);
    sweep_counter_.enable_reload_flag();
    sweep_positive_ = -2*(b >> 3 & 1) + 1;  // -1 if negate flag, 1 if not
    sweep_shift_ = b & 7;
}

void Pulse::Set4002(uint8_t b) {  // period low
    timer_counter_.set_reload((timer_counter_.reload() | 255) & b);
}

void Pulse::Set4003(uint8_t b) {  // length counter bitload, period high
    timer_counter_.set_reload((timer_counter_.reload() | 1729) & ((b & 7) << 8));
    if (enabled_)
        length_counter.set_value(self.LENGTH_COUNTER_TABLE[b >> 3]);
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
    int16_t shift_result = timer_Counter_.reload() >> sweep_shift_;
    shift_result *= sweep_positive_;
    if(sweep_positive < 0 and isPulse1) shift_result--;
    return timer_counter_.reload() + shift_result;
}

void Pulse::SweepAdjustPeriod() {
    if (sweep_shift)
        timer_counter_.set_reload(SweepGetTarget());
}

void Pulse::ChannelSpecificCounterCallback(Counter *c) {
    if (c == &sweep_counter_)
        SweepAdjustPeriod();
    else if (c == &timer_counter_)
        SequenceClock();
}

void Pulse::CounterReloadCallback(Counter *c) {
    if (c == &sweep_counter_)
        SweepAdjustPeriod();
}
