#ifndef NOISE_H
#define NOISE_H

#include "channel.h"

class Noise : Channel {
 public:
    Noise() {};

    void Set400C(uint8_t b);
    void Set400E(uint8_t b);
    void Set400F(uint8_t b);
    
 private:
    const uint16_t kPeriodTable = 
            [   4,    8,   16,   32,   64,   96,  128,  160,
              202,  254,  380,  508,  762, 1016, 2034, 4068];

    bool mode_flag = false;
    uint16 shift_register_ = 1;

    void ShiftRegisterClock();
};

#endif
