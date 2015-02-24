#include "apu.h"

void APU::SetByte(uint16_t addr, uint8_t b) {
    if (addr >= 0x4000 && addr <= 0x4003)
        pulse1_.SetByte(addr, b);
    if (addr >= 0x4004 && addr <= 0x4007)
        pulse2_.SetByte(addr, b);
    if (addr >= 0x4008 && addr <= 0x400B)
        triangle_.SetByte(addr, b);
    if (addr >= 0x400C && addr <= 0x400F)
        noise_.SetByte(addr, b);
    if (addr >= 0x4010 && addr <= 0x4013)
        dmc_.SetByte(addr, b);
    if (addr == 0x4015)
        Set4015(b);
    if (addr == 0x4017)
        Set4017(b);
}

void APU::Set4015(uint8_t b) {  // enable individual channels
    for(int i = 0; i < 5; i++) {
        channels_[i]->SetEnabled(b & 1);
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
        b += channels_[i]->LengthCounterEnabled();
        b <<= 1;
    }
    b += dmc_.active() << 4;
    b += dmc_.irq() << 6;
    
    // TODO: frame counter IRQ

    return b;
}

void APU::CPUClock() {
    fc_reset_timer_.Clock();
    fc_half_clock_.Clock();
    fc_quarter_clock_.Clock();
    fc_divider_.Clock();
    if (!fc_divider_.value())
        triangle_.TimerClock();
}

void APU::GetCurrent(vector<uint8_t>& output) {
    output.clear();
    for(int i = 0; i < 5; i++)
        output.push_back(channels_[i]->GetCurrent());
}

void APU::FCReset() {
    fc_half_clock_.set_value(0);
    fc_quarter_clock_.set_value(0);
}

void APU::FCQuarterClock() {
    for(int i = 0; i < 4; i++) 
        channels_[i]->EnvelopeClock();
    triangle_.LinearCounterClock();
}

void APU::FCHalfClock() {
    for(int i = 0; i < 4; i++) {
        channels_[i]->LengthCounterClock();
        channels_[i]->SweepClock();
    }
}

void APU::APUClock() {
    for(int i = 0; i < 5; i++)
        channels_[i]->TimerClock();
}

void APU::CounterCallback(Counter *c) {
    if (c == &fc_divider_)
        APUClock();
    else if (c == &fc_reset_timer_)
        FCReset();
    else if (c == &fc_half_clock_)
        FCHalfClock();
    else if (c == &fc_quarter_clock_)
        FCQuarterClock();
}
