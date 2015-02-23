#ifndef PULSE_H
#define PULSE_H

#import "channel.h"

class Pulse : public Channel {
 public:
    Pulse(bool isPulse1) {sequencer_counter_.set_reload(7);}

    void Set4000(uint8_t b);
    void Set4001(uint8_t b);
    void Set4002(uint8_t b);
    void Set4003(uint8_t b);

    uint8_t GetCurrent();
    
 private:
    const uint8_t kSeq0[8] = {0, 1, 0, 0, 0, 0, 0, 0};  // 12.5%
    const uint8_t kSeq1[8] = {0, 1, 1, 0, 0, 0, 0, 0};  // 25%
    const uint8_t kSeq2[8] = {0, 1, 1, 1, 1, 0, 0, 0};  // 50%
    const uint8_t kSeq3[8] = {1, 0, 0, 1, 1, 1, 1, 1};  // 25% negated
    const uint8_t* kSequences[4] = {kSeq0, kSeq1, kSeq2, kSeq3};

    uint8_t duty_ = 0;  // range is 0 - 3

    int8_t sweep_positive_ = 1;  // -1 or 1
    uint8_t sweep_shift_ = 0;  // range is 0 - 7
        
    bool isPulse1_ = true;  // to account for a small difference in sweep unit

    int SweepGetTarget();
    void SweepAdjustPeriod();
    void CounterReloadCallback(Counter *c);
    void ChannelSpecificCounterCallback(Counter *c);
};

#endif
