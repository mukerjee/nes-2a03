#ifndef APU_H
#define APU_H

#include "countable.h"

#include "pulse.h"
#include "triangle.h"
#include "noise.h"
#include "dmc.h"

#include "counter.h"

#include "apu_mixer.h"
#include "resample.h"

#include <vector>

class APU : Countable {
 public:
 APU() : pulse1_(true), pulse2_(false), fc_half_clock_(14914, true, this, false),
        fc_quarter_clock_(7457, true, this, false), fc_divider_(1, true, this),
        fc_reset_timer_(0, false, this) {}

    void SetByte(uint16_t addr, uint8_t b);
    uint8_t GetByte(uint16_t addr);

    void CPUClock();
    
    void GetCurrent(vector<uint8_t>& output);

 private:

    Pulse pulse1_;
    Pulse pulse2_;
    Triangle triangle_;
    Noise noise_;
    DMC dmc_;
    Channel *channels_[5] = {&pulse1_, &pulse2_, &triangle_, &noise_, &dmc_};

    // Frame Counter
    // TODO: Implement 5-step mode and IRQ
    // These are in terms of CPU cycles, not APU
    Counter fc_half_clock_;
    Counter fc_quarter_clock_;
    Counter fc_divider_;
    Counter fc_reset_timer_;

    void Set4015(uint8_t b);
    void Set4017(uint8_t b);

    uint8_t Get4015();

    void FCReset();
    void FCQuarterClock();
    void FCHalfClock();
    void APUClock();

    void CounterCallback(Counter *c);
    void CounterReloadCallback(Counter *c) {}
};

#endif
