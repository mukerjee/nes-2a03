#ifndef PULSE_H
#define PULSE_H

#import "channel.h"

class Pulse : Channel {
 public:
    Pulse(bool isPulse1) {sequence_counter_.set_reload(7);}

    void Set4000(uint8_t b);
    void Set4001(uint8_t b);
    void Set4002(uint8_t b);
    void Set4003(uint8_t b);
    
 private:
    const uint8_t kSeq0 = [0, 1, 0, 0, 0, 0, 0, 0];  // 12.5%
    const uint8_t kSeq1 = [0, 1, 1, 0, 0, 0, 0, 0];  // 25%
    const uint8_t kSeq2 = [0, 1, 1, 1, 1, 0, 0, 0];  // 50%
    const uint8_t kSeq3 = [1, 0, 0, 1, 1, 1, 1, 1];  // 25% negated
    const uint8_t* kSequences = [kSeq0, kSeq1, kSeq2, kSeq3];

    uint8_t duty_ = 0;  // range is 0 - 3

    int8_t sweep_postive = 1;  // -1 or 1
    uint8_t sweep_shift_ = 0;  // range is 0 - 7
        
    bool isPulse1_ = true;  // to account for a small difference in sweep unit

    int SweepGetTarget();
    void SweepAdjustPeriod();
};

#endif
