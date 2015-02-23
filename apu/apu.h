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

    void Set4000(uint8_t b) {pulse1_.Set4000(b);}
    void Set4001(uint8_t b) {pulse1_.Set4001(b);}
    void Set4002(uint8_t b) {pulse1_.Set4002(b);}
    void Set4003(uint8_t b) {pulse1_.Set4003(b);}

    void Set4004(uint8_t b) {pulse2_.Set4000(b);}
    void Set4005(uint8_t b) {pulse2_.Set4001(b);}
    void Set4006(uint8_t b) {pulse2_.Set4002(b);}
    void Set4007(uint8_t b) {pulse2_.Set4003(b);}

    void Set4008(uint8_t b) {triangle_.Set4008(b);}
    void Set400A(uint8_t b) {triangle_.Set400A(b);}
    void Set400B(uint8_t b) {triangle_.Set400B(b);}

    void Set400C(uint8_t b) {noise_.Set400C(b);}
    void Set400E(uint8_t b) {noise_.Set400E(b);}
    void Set400F(uint8_t b) {noise_.Set400F(b);}

    void Set4010(uint8_t b) {dmc_.Set4010(b);}
    void Set4011(uint8_t b) {dmc_.Set4011(b);}
    void Set4012(uint8_t b) {dmc_.Set4012(b);}
    void Set4013(uint8_t b) {dmc_.Set4013(b);}

    void Set4015(uint8_t b);
    void Set4017(uint8_t b);
    
    uint8_t Get4015();

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

    void FCReset();
    void FCQuarterClock();
    void FCHalfClock();
    void APUClock();

    void CounterCallback(Counter *c);
    void CounterReloadCallback(Counter *c) {}
};

#endif
