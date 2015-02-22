#ifndef PULSE_H
#define PULSE_H

using namespace std;

class Pulse {
 public:
    Pulse(bool isPulse1) {};

    void SetEnabled(bool enabled);
    
    void Set4000(uint8_t b);
    void Set4001(uint8_t b);
    void Set4002(uint8_t b);
    void Set4003(uint8_t b);
    
    void LengthCounterClock() {length_counter_.Clock();}  // called in half frames
    void EnvelopeClock();
    void SweepClock() {sweep_counter_.Clock();}
    void TimerClock() {timer_counter_.Clock();}  // called every other CPU clock
    uint8_t GetCurrent();
    bool LengthCounterEnabled() {return length_counter_.value() > 0;}
    
 private:
    const uint8_t kSeq0 = [0, 1, 0, 0, 0, 0, 0, 0];  // 12.5%
    const uint8_t kSeq1 = [0, 1, 1, 0, 0, 0, 0, 0];  // 25%
    const uint8_t kSeq2 = [0, 1, 1, 1, 1, 0, 0, 0];  // 50%
    const uint8_t kSeq3 = [1, 0, 0, 1, 1, 1, 1, 1];  // 25% negated
    const uint8_t* kSequences = [kSeq0, kSeq1, kSeq2, kSeq3];

    const uint8_t kLengthCounterTable =
            [10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
             12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30];

    uint8_t duty_ = 0;  // range is 0 - 3

    int8_t sweep_postive = 1;  // -1 or 1
    uint8_t sweep_shift_ = 0;  // range is 0 - 7

    uint8_t volume_ = 0;  // range is 0 - F
        
    Counter length_counter_(0, false);
    Counter envelope_divider_(0, true, &envelope_counter.Clock());
    Counter envelope_counter_(15, false);
    Counter sweep_counter_(0, true, &SweepAdjustPeriod, true, &SweepAdjustPeriod);
    Counter timer_counter_(0, true, &SequenceClock); // range is 0 - 7FF
    Counter sequencer_counter_(7);

    bool enabled_ = 0;
    bool isPulse1_ = true;  // to account for a small difference in sweep unit

    int SweepGetTarget();
    void SweepAdjustPeriod();

    void SequencerClock() {sequencer_counter_.Clock();}
};

#endif
