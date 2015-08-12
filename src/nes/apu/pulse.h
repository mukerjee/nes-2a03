#ifndef NES_APU_PULSE_H_
#define NES_APU_PULSE_H_

#include <stdint.h>

#include "channel.h"

class Pulse : public Channel {
 public:
 Pulse(bool is_pulse_1) : is_pulse_1_(is_pulse_1) {}

    void SetEnabled(bool enabled);
    void SetByte(uint16_t addr, uint8_t b);

    void TimerClock();
    void EnvelopeClock();
    void LengthClock();
    void SweepClock();

    uint8_t GetCurrent();
    
 private:
    static const uint8_t kSequenceLength = 8;
    const uint8_t kSeq0[kSequenceLength] =
        {0, 1, 0, 0, 0, 0, 0, 0};  // 12.5%
    const uint8_t kSeq1[kSequenceLength] =
        {0, 1, 1, 0, 0, 0, 0, 0};  // 25%
    const uint8_t kSeq2[kSequenceLength] =
        {0, 1, 1, 1, 1, 0, 0, 0};  // 50%
    const uint8_t kSeq3[kSequenceLength] =
        {1, 0, 0, 1, 1, 1, 1, 1};  // 25% negated
    const uint8_t* kSequences[4] =
        {kSeq0, kSeq1, kSeq2, kSeq3};

    bool enabled_ = false;

    bool is_pulse_1_ = true;  // small difference in sweep

    Divider timer_ = {.counter = 0, .reload = 0};
    Divider envelope_ = {.counter = 0, .reload = 0};
    uint8_t length_ = 10;
    Divider sweep_ = {.counter = 0, .reload = 0};

    bool constant_volume_flag_ = false;
    uint8_t constant_volume_ = 0;
    uint8_t envelope_volume_ = 0;
    bool envelope_start_flag_ = false;

    bool sweep_enabled_ = false;
    int8_t sweep_mode_ = 1;  // -1, 1
    uint8_t sweep_shift_ = 0;  // range is 0 - 7
    bool sweep_reload_flag_ = false;

    uint8_t duty_ = 0;  // range is 0 - 3
    uint8_t sequencer_ = 0;
    bool control_flag_ = false;

    int SweepGetTarget();
};

#endif  // NES_APU_PULSE_H_
