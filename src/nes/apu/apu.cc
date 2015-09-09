#include "apu.h"

Apu::Apu(Nes *nes) {
    pulse1_ = new Pulse(true);
    pulse2_ = new Pulse(false);
    triangle_ = new Triangle();
    noise_ = new Noise();
    dmc_ = new DMC(nes);

    channels_ = (Channel **) malloc(5 * sizeof(Channel**));
    channels_[0] = pulse1_;
    channels_[1] = pulse2_;
    channels_[2] = triangle_;
    channels_[3] = noise_;
    channels_[4] = dmc_;
    
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
        pulse1_->SetByte(addr, b);
    if (addr >= 0x4004 && addr <= 0x4007)
        pulse2_->SetByte(addr, b);
    if (addr >= 0x4008 && addr <= 0x400B)
        triangle_->SetByte(addr, b);
    if (addr >= 0x400C && addr <= 0x400F)
        noise_->SetByte(addr, b);
    if (addr >= 0x4010 && addr <= 0x4013)
        dmc_->SetByte(addr, b);
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

uint8_t Apu::Get4015() {  // TODO: Frame counter / DMC status
    uint8_t p1 = pulse1_->length();
    uint8_t p2 = pulse2_->length();
    uint8_t t = triangle_->length();
    uint8_t n = noise_->length();
    uint8_t d = dmc_->bytes_remaining();

    uint8_t result = 0;
    result += d ? 1 : 0;
    result <<= 1;
    result += n ? 1 : 0;
    result <<= 1;
    result += t ? 1 : 0;
    result <<= 1;
    result += p2 ? 1 : 0;
    result <<= 1;
    result += p1 ? 1 : 0;

    return result; // | 0x40;
}

float Apu::GetSample() {
    float pul = pulse_lookup[pulse1_->GetCurrent() + pulse2_->GetCurrent()];
    float tnd = tnd_lookup[3*triangle_->GetCurrent() + 2*noise_->GetCurrent()
                           + dmc_->GetCurrent()];
    float sample = 2 * (pul + tnd) - 1;


    // filter
    // High pass at 90Hz
    float x = 0.987259; // e ^ (-2 * pi * (90 / 44100))
    float prev_hp90 = hp90_buf_;
    if (HP_90_ENABLED) {
        hp90_buf_ = ((1 + x) / 2) * sample - ((1 + x) / 2) * prev_sample_
            + x * hp90_buf_;
    } else {
        hp90_buf_ = sample;
    }

    // High pass at 440Hz
    x = 0.939235;
    if (HP_440_ENABLED) {
        hp440_buf_ = ((1 + x) / 2) * hp90_buf_ - ((1 + x) / 2) * prev_hp90
            + x * hp440_buf_;
    } else {
        hp440_buf_ = hp90_buf_;
    }

    // Low pass at 14400Hz
    x = 0.136059; // e ^ (-2 * pi * (14000 / 44100))
    if (LP_14000_ENABLED) {
        lp14000_buf_ = (1-x) * hp440_buf_ + x * lp14000_buf_;
    } else {
        lp14000_buf_ = hp440_buf_;
    }

    prev_sample_ = sample;
    return lp14000_buf_;
}


void Apu::QuarterClock() {
    for (int i = 0; i < 5; i++)
        channels_[i]->EnvelopeClock();
    triangle_->LinearClock();
}

void Apu::HalfClock() {
    for (int i = 0; i < 5; i++)
        channels_[i]->LengthClock();
    pulse1_->SweepClock();
    pulse2_->SweepClock();
}

void Apu::ClockCycles(int cycles) {
    for(int i = 0; i < cycles; i++) {
        frame_counter_++;

        if (frame_counter_ % 2 == 0)  // APU Clock
            for (int i = 0; i < 5; i++)
                channels_[i]->TimerClock();
        else { // CPU Clock
            triangle_->TimerClock();
            dmc_->TimerClock();
        }

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
