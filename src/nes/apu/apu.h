#ifndef NES_APU_APU_H_
#define NES_APU_APU_H_

#include <stdint.h>

#include "gallant_signal.h"
#include "audio_source.h"

#include "channel.h"
#include "pulse.h"
#include "triangle.h"
#include "noise.h"
#include "dmc.h"

class Apu : public AudioSource {
 public:
    Apu();

    void SetByte(uint16_t addr, uint8_t b);
    float GetSample();
    void ClockCycles(int cycles);
    Gallant::Signal1<int> RanCycles;
    
 private:
    Pulse pulse1_;
    Pulse pulse2_;
    Triangle triangle_;
    Noise noise_;
    DMC dmc_;
    Channel *channels_[5] = {&pulse1_, &pulse2_, &triangle_, &noise_, &dmc_};

    float pulse_lookup[31];
    float tnd_lookup[203]; // This is approximate (within 4% of DMC)
    float hp90_buf_, hp440_buf_, lp14000_buf_;

    // TODO: Implement 5-step mode and IRQ
    uint16_t frame_counter_;  // These are in terms of CPU cycles, not Apu
    void QuarterClock();
    void HalfClock();

    // uint8_t Get4015();  // TODO: implement
};

#endif  // NES_APU_APU_H_
