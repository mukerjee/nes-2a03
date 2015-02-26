#ifndef TRIANGLE_H
#define TRIANGLE_H

#import "channel.h"

class Triangle : public Channel {
 public:
 Triangle() : linear_counter_(0, false) {
        sequencer_counter_.set_reload(31); sequencer_counter_.set_down(false);};

    void SetByte(uint16_t addr, uint8_t b);

    void LinearCounterClock() {linear_counter_.Clock();}
    void EnvelopeClock() {}
    uint8_t GetCurrent();

 private:
    // F E D C B A 9 8 7 6 5 4 3 2 1 0 0 1 2 3 4 5 6 7 8 9 A B C D E F
    const int8_t kSequence[32] =
        {15, 14, 13, 12, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0,
          0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15};

    void Set4008(uint8_t b);
    void Set400A(uint8_t b);
    void Set400B(uint8_t b);

    bool linear_counter_control_flag_ = false;

    Counter linear_counter_; // range is 0 - 127

    void SequencerClock();
    void ChannelSpecificCounterCallback(Counter *c);
};

#endif
