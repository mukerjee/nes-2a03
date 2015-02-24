#ifndef NOISE_H
#define NOISE_H

#include "channel.h"

class Noise : public Channel {
 public:
    Noise() {};

    void SetByte(uint16_t addr, uint8_t b);

    uint8_t GetCurrent();
    
 private:
    const uint16_t kPeriodTable[16] = 
        {   4,    8,   16,   32,   64,   96,  128,  160,
            202,  254,  380,  508,  762, 1016, 2034, 4068};

    bool mode_flag_ = false;
    uint16_t shift_register_ = 1;

    void Set400C(uint8_t b);
    void Set400E(uint8_t b);
    void Set400F(uint8_t b);

    void ShiftRegisterClock();
    void ChannelSpecificCounterCallback(Counter *c);
};

#endif
