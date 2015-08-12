#include "nes.h"

Nes::Nes(int sample_rate) :
    sample_rate_(sample_rate),
    audio_adapter_(&apu_, CLOCK_SPEED, sample_rate) {
    cpu_ = new Cpu(this, CLOCK_SPEED);
    cpu_->RanCycles.Connect(&apu_, &Apu::ClockCycles);
    apu_.RanCycles.Connect(&audio_adapter_, &AudioAdapter::ClockCycles);
    audio_adapter_.OutputSample.Connect(this, &Nes::CountSamples);

};

void Nes::Run() {
    for(;;) {
        while(number_of_samples_ < sample_rate_ * 5) {
            if (cycles_since_last_period_ >= periodic_) {
                cycles_since_last_period_ -= periodic_;
                cycles_since_last_period_ += cpu_->Run();
            } else {
                apu_.ClockCycles(periodic_ - cycles_since_last_period_);
                cycles_since_last_period_ = periodic_;
            }
        }
        // pause periodically...
        usleep(2.5 * 1000000);
        number_of_samples_ -= sample_rate_ * 5;
    }
}

void Nes::SetRegisters(uint8_t a, uint8_t x, uint8_t y, uint8_t s, uint16_t pc) {
    cpu_->register_a_ = a;
    cpu_->register_x_ = x;
    cpu_->register_y_ = y;
    cpu_->register_s_ = s;
    cpu_->register_pc_ = pc;
}

void Nes::SetByte(const uint16_t addr, const uint8_t byte) {
    if (addr >= 0x5FF8 and addr <= 0x5FFF) { // bank switching
        memory_.BankSwitch(addr - 0x5FF8, byte); 
    } else
    if (addr >= 0x4000 and addr <= 0x4017) { // APU
        apu_.SetByte(addr, byte);
    } else {
        memory_.SetByte(addr, byte);
    }
}

uint8_t Nes::GetByte(const uint16_t addr) {
    return memory_.GetByte(addr);
}

uint16_t Nes::GetWord(const uint16_t addr) {
    return memory_.GetWord(addr);
}
