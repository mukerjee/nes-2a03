#include "apu.h"

void APU::Set4015(uint8_t b) {  // enable individual channels
    for(int i = 0; i < 5; i++) {
        channels_[i].SetEnabled(b & 1);
        b >>= 1;
    }
}
 
void APU::Set4017(uint8_t b) {  // frame counter
    // TODO: lots of frame counter things
    fc_reset_timer_.set_value(4);
    if (fc_divider_.value())  // middle of APU cycle
        fc_reset_timer_.set_value(3);
} 
 
uint8_t APU::Get4015() {
    uint8_t b = 0;
    for(int i = 3; i >= 0; i--) {
        b += channels_[i].get_length_counter();
        b <<= 1;
    }
    b += self.dmc.get_active() << 4;
    i += self.dmc.get_interrupt() << 6;
    
    // TODO: frame counter IRQ

    return b;
}

void APU::FCQuarterClock() {
    for(int i = 0; i < 4; i++) 
        channels_[i].EnvelopeClock();
    triangle_.LinearCounterClock();
}

void APU::FCHalfClock() {
    for(int i = 0; i < 4; i++) {
        channels_[i].LengthCounterClock();
        channels_[i].SweepClock();
    }
}

void APU::FCReset() {
    fc_half_clock_.set_value(0);
    fc_quarter_clock_.set_value(0);
}

void APU::APUClock() {
    for(int i = 0; i < 5; i++)
        channels_[i].TimerClock();
}

void APU::CPUClock() {
    fc_reset_timer.Clock();
    fc_half_clock_.Clock();
    fc_quarter_clock_.Clock();
    fc_divider_.Clock();
    if (!fc_divider_.value())
        triangle_.TimerClock();
}

void APU::GetCurrent(vector<uint8_t*>& output) {
    output.clear();
    for(int i = 0; i < 5; i++)
        output.push_back(channels_[i].GetCurrent());
}

void APU::CounterCallback(Counter *c) {
    switch(c) {
    case &fc_divider_:
        APUClock();
        break;
    case &fc_reset_timer_:
        FCReset();
        break;
    case &fc_half_clock_:
        FCHalfClock();
        break;
    case &fc_quarter_clock_:
        FCQuarterClock();
        break;
    }
}

void APU::CounterReloadCallback(Counter *c) {}
