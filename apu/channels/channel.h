#ifndef CHANNEL_H
#define CHANNEL_H

#include "counter.h"
#include "countable.h"
#include <stdint.h>

class Channel : Countable {
 public:
 Channel() : linear_counter_(0, false), length_counter_(0, false), 
        envelope_divider_(0, true, this), envelope_counter_(15, false), 
        sweep_counter_(0, true, this, true, this), timer_counter_(0, true, this), 
        sequencer_counter_(0, true) {}

    void SetEnabled(bool enabled);

    void LinearCounterClock() {linear_counter_.Clock();}
    void LengthCounterClock() {length_counter_.Clock();}
    void EnvelopeClock();
    void SweepClock() {sweep_counter_.Clock();}
    void TimerClock() {timer_counter_.Clock();}
    
    virtual uint8_t GetCurrent() = 0;
    bool LengthCounterEnabled() {return length_counter_.value() > 0;}

 protected:
    const uint8_t kLengthCounterTable[32] = 
        {10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
         12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

    uint8_t volume_ = 0;  // range is 0 - F

    Counter linear_counter_; // range is 0 - 127
    Counter length_counter_;
    Counter envelope_divider_;
    Counter envelope_counter_;
    Counter sweep_counter_;
    Counter timer_counter_; // range is 0 - 7FF
    Counter sequencer_counter_;

    bool enabled_ = 0;

    virtual void SequencerClock() {sequencer_counter_.Clock();}
    void CounterCallback(Counter *c);
    virtual void CounterReloadCallback(Counter *c) {};
    virtual void ChannelSpecificCounterCallback(Counter *c) {};
};

#endif
