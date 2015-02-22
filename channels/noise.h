#ifndef NOISE_H
#define NOISE_H

using namespace std;

class Noise {
 public:
    Noise() {};

    void SetEnabled(bool enabled);
    
    void Set400C(uint8_t b);
    void Set400E(uint8_t b);
    void Set400F(uint8_t b);
    
    void LengthCounterClock() {length_counter_.Clock();}  // called in half frames
    void EnvelopeClock();
    void TimerClock() {timer_counter_.Clock()}  // called every other CPU clock
    uint8_t GetCurrent();
    bool LengthCounterEnabled() {return length_counter_.value() > 0;}
    
 private:
    const uint8_t kLengthCounterTable =
            [10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
             12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30];

    const uint16_t kPeriodTable = 
            [   4,    8,   16,   32,   64,   96,  128,  160,
              202,  254,  380,  508,  762, 1016, 2034, 4068];


    uint8_t volume_ = 0;  // range is 0 - F
        
    bool mode_flag = false;
    uint16 shift_register_ = 1;

    Counter length_counter_(0, false);
    Counter envelope_divider_(0, true, &envelope_counter.Clock());
    Counter envelope_counter_(15, false);
    Counter timer_counter_(0, true, &ShiftRegisterClock); // range is 0 - 7FF

    bool enabled_ = 0;

    void ShiftRegisterClock();
};

#endif
