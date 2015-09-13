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
}

Apu::~Apu() {
    free(channels_);
    
    delete pulse1_;
    delete pulse2_;
    delete triangle_;
    delete noise_;
    delete dmc_;
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

void Apu::GetCurrent(uint8_t &p1, uint8_t &p2, uint8_t &t, uint8_t &n, uint8_t &d) {
    p1 = pulse1_->GetCurrent();
    p2 = pulse2_->GetCurrent();
    t = triangle_->GetCurrent();
    n = noise_->GetCurrent();
    d = dmc_->GetCurrent();
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
