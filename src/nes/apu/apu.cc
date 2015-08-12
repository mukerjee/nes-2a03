#include "apu.h"

Apu::Apu() : pulse1_(true), pulse2_(false) {
    hp90_buf_ = hp440_buf_ = lp14000_buf_ = 0.0;
    pulse_lookup[0] = 0.0;
    for (int i = 1; i < 31; i++)
        pulse_lookup[i] = 95.52 / (8128.0 / i + 100);

    tnd_lookup[0] = 0.0;
    for (int i = 1; i < 203; i++)
        tnd_lookup[i] = 163.67 / (24329.0 / i + 100);
}

void Apu::SetByte(uint16_t addr, uint8_t b) {
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
    if (addr == 0x4015) {
        for(int i = 0; i < 5; i++) {  // enable individual channels
            channels_[i]->SetEnabled(b & 1);
            b >>= 1;
        }
    }
    if (addr == 0x4017) {  // frame counter
        // TODO: lots of frame counter things
        frame_counter_ = (frame_counter_ % 2 == 0) ? 29829 - 4 : 29829 - 3;
    }
}

float Apu::GetSample() {
    float pul = pulse_lookup[pulse1_.GetCurrent() + pulse2_.GetCurrent()];
    float tnd = tnd_lookup[3*triangle_.GetCurrent() + 2*noise_.GetCurrent()
                           + dmc_.GetCurrent()];
    float sample = 2 * (pul + tnd) - 1;

    // TODO: fix
    // filter
    // hp90_buf_ += 90 * (sample - hp90_buf_);
    // hp440_buf_ += 440 * ((sample - hp90_buf_) - hp440_buf_);
    // lp14000_buf_ += 14000 * ((sample - hp440_buf_) - lp14000_buf_);
    return sample; // lp14000_buf_;
}


void Apu::QuarterClock() {
    for (int i = 0; i < 5; i++)
        channels_[i]->EnvelopeClock();
    triangle_.LinearClock();
}

void Apu::HalfClock() {
    for (int i = 0; i < 5; i++)
        channels_[i]->LengthClock();
    pulse1_.SweepClock();
    pulse2_.SweepClock();
}

void Apu::ClockCycles(int cycles) {
    for(int i = 0; i < cycles; i++) {
        frame_counter_++;

        if (frame_counter_ % 2 == 0)  // APU Clock
            for (int i = 0; i < 5; i++)
                channels_[i]->TimerClock();
        else  // CPU Clock
            triangle_.TimerClock();

        // Frame Counter
        switch(frame_counter_) {
        case 7457: // quarter
            QuarterClock();
            break;
        case 14913: // half
            QuarterClock();
            HalfClock();
            break;
        case 22371: // three-quarters
            QuarterClock();
            break;
        case 29829: // full
            QuarterClock();
            HalfClock();
            frame_counter_ = 0;
            break;
        }
        RanCycles(1);
    }    
}
