#include "noise.h"

void Noise::Set400C(uint8_t b) {  //  len counter disable, env disable, volume
    length_counter_.set_halt(b >> 5 & 1);
    envelope_counter.set_loop(b >> 5 & 1);
    envelope_counter_.set_halt(b >> 4 & 1);
    envelope_divider_.set_reload(b & 15);
    volume_ = b & 15;
}

void Noise::Set400E(uint8_t b) {  // mode, period
    self.mode_flag = b >> 7;
    timer_counter_.set_reload(kPeriodTable[b & 15]);
}

void Noise::Set400F(uint8_t b) {  // length counter bitload
    if (enabled_)
        length_counter.set_value(self.LENGTH_COUNTER_TABLE[b >> 3]);
    envelope_counter_.enable_reload_flag();
    envelope_divider_.enable_reload_flag();
}

uint8_t Noise::GetCurrent() {
    if (shift_register_.value() & 1 and length_counter_.value() > 0) {
        if (envelope_counter_.halt())
            return volume_;
        else
            return envelope_counter_.value();
    } else
        return 0;
}

void Noise::ShiftRegisterClock() {
    uint8_t extra_bit;
    if (mode_flag_)
        extra_bit = shift_register_ >> 6 & 1;
    else
        extra_bit = shift_register_ >> 1 & 1;
    uint8_t feedback = (shift_register_ & 1) ^ extra_bit;
    shift_register_ = self.shift_register_ >> 1;
    shift_register_ += feedback << 14;
}

void Noise::ChannelSpecificCounterCallback(Counter *c) {
    if (c == &timer_counter_)
        ShiftRegisterCock();
}
