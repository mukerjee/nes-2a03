#ifndef NES_APU_TRIANGLE_H_
#define NES_APU_TRIANGLE_H_

#include <stdint.h>

#include "channel.h"

class Triangle : public Channel {
 public:
    void SetEnabled(bool enabled);
    void SetByte(uint16_t addr, uint8_t b);

    void TimerClock();
    void EnvelopeClock() {}
    void LengthClock();
    void LinearClock();

    uint8_t GetCurrent() {return kSequence[sequencer_];}

    uint8_t length() {return length_;}
    
 private:
    static const uint8_t kSequenceLength = 32;
    // F E D C B A 9 8 7 6 5 4 3 2 1 0
    // 0 1 2 3 4 5 6 7 8 9 A B C D E F
    const uint8_t kSequence[kSequenceLength] =
        {15, 14, 13, 12, 11, 10,  9,  8,
         7,  6,  5,  4,  3,  2,  1,  0,
         0,  1,  2,  3,  4,  5,  6,  7,
         8,  9, 10, 11, 12, 13, 14, 15};

    bool enabled_ = false;

    Divider timer_ = {.counter = 0, .reload = 0};
    uint8_t length_ = 0;
    Divider linear_ = {.counter = 0, .reload = 0};
    uint8_t sequencer_ = 0;

    bool linear_reload_flag_ = false;
    bool control_flag_ = false;
};

#endif  // NES_APU_TRIANGLE_H_
