#ifndef NES_APU_APU_H_
#define NES_APU_APU_H_

#include <stdint.h>

#include "gallant_signal.h"
#include "audio_source.h"

#include "nes.h"

#include "channel.h"
#include "pulse.h"
#include "triangle.h"
#include "noise.h"
#include "dmc.h"

#define HP_90_ENABLED 0
#define HP_440_ENABLED 0
#define LP_14000_ENABLED 1

class Nes;
class DMC;

class Apu : public AudioSource {
 public:
    Apu(Nes *nes);

    void SetByte(uint16_t addr, uint8_t b);
    uint8_t Get4015();  // TODO: finish implementation
    float GetSample();

    void ClockCycles(int cycles);  
    Gallant::Signal1<int> RanCycles;
    
 private:
    Pulse *pulse1_;
    Pulse *pulse2_;
    Triangle *triangle_;
    Noise *noise_;
    DMC *dmc_;
    Channel **channels_;

    float pulse_lookup[31];
    float tnd_lookup[203]; // This is approximate (within 4% of DMC)
    float hp90_buf_, hp440_buf_, lp14000_buf_, prev_sample_;

    // TODO: Implement 5-step mode and IRQ
    uint16_t frame_counter_;  // These are in terms of CPU cycles, not Apu
    void QuarterClock();
    void HalfClock();
};

#endif  // NES_APU_APU_H_
