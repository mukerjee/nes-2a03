#ifndef APU_H
#define APU_H

#include "pulse.h"
#include "triangle.h"
#include "noise.h"
#include "dmc.h"

#include "apu_mixer.h"
#include "resample.h"

#include <vector>

class APU {
 public:
    APU() {}

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
    
    void GetCurrent(vector<uint8_t*>& output);

 private:

    Pulse pulse1_(true);
    Pulse pulse2_(false);
    Triangle triangle_();
    Noise noise_();
    DMC dmc_();

    // Frame Counter
    bool fc_irq_inhibit_ = false;
    bool fc_irq_ = false;
    bool fc_mode_five_step_ = false;
    bool fc_please_quarter_clock_ = false;
    bool fc_please_half_clock_ = false;
    Counter fc_divider_(1, true, &APUClock);
    Counter fc_sequencer_(0);
    Counter fc_reset_timer(0, false, &FCReset);

    void FCQuarterClock();
    void FCHalfClock();
    void APUClock();
};

#endif
