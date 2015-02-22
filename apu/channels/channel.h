#ifndef CHANNEL_H
#define CHANNEL_H

#include "counter.h"
#include "countable.h"

using namespace std;

class Channel : Countable {
 public:
    Channel() {}

    void SetEnabled(bool enabled);

    void LinearCounterClock() {linear_counter_.Clock();}
    void LengthCounterClock() {length_counter_.Clock();}
    void EnvelopeClock();
    void SweepClock() {sweep_counter_.Clock();}
    void TimerClock() {timer_counter_.Clock();}
    
    uint8_t GetCurrent();
    bool LengthCounterEnabled() {return length_counter_.value() > 0;}

 protected:
    const uint8_t kLengthCounterTable =
            [10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
             12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30];

    uint8_t volume_ = 0;  // range is 0 - F

    Counter linear_counter_(0, false); // range is 0 - 127
    Counter length_counter_(0, false);
    Counter envelope_divider_(0, true, this);
    Counter envelope_counter_(15, false);
    Counter sweep_counter_(0, true, this, true, this);
    Counter timer_counter_(0, true, this); // range is 0 - 7FF
    Counter sequencer_counter_(0, true);

    bool enabled_ = 0;

    virtual void ChannelSpecificCounterCallback(Counter *c);
    void SequencerClock() {sequencer_counter_.Clock();}
}
