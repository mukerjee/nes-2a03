#ifndef TRIANGLE_H
#define TRIANGLE_H

using namespace std;

class Triangle {
 public:
    Triangle() {};

    void SetEnabled(bool enabled);
    
    void Set4008(uint8_t b);
    void Set400A(uint8_t b);
    void Set400B(uint8_t b);
    
    void LinearCounterClock() {linear_counter_.Clock();}  // called in quarter frames
    void LengthCounterClock() {length_counter_.Clock();}  // called in half frames
    void TimerClock() {timer_counter_.Clock()};  // called every CPU clock
    uint8_t GetCurrent() {return kSequence[sequencer_counter_.value()];}
    bool LengthCounterEnabled() {return length_counter_.value() > 0;}
    
 private:
    // F E D C B A 9 8 7 6 5 4 3 2 1 0 0 1 2 3 4 5 6 7 8 9 A B C D E F
    const int8_t kSequence =
        [15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15];
    const uint8_t kLengthCounterTable =
            [10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
             12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30];

    Counter linear_counter_(0, false); // range is 0 - 127
    Counter length_counter_(0, false);
    Counter timer_counter_(0, true, &SequenceClock); // range is 0 - 7FF
    Counter sequencer_counter_(31, true, NULL, false);

    bool enabled_ = 0;

    void SequencerClock();
};

#endif
