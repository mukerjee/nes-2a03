#ifndef NES_APU_APU_H_
#define NES_APU_APU_H_

#include <stdint.h>

#include "gallant_signal.h"

#include "nes.h"

#include "channel.h"
#include "pulse.h"
#include "triangle.h"
#include "noise.h"
#include "dmc.h"

class Nes;
class DMC;

class Apu {
 public:
    Apu(Nes *nes);
    ~Apu();

    void SetByte(uint16_t addr, uint8_t b);
    uint8_t Get4015();  // TODO: finish implementation
    void GetCurrent(uint8_t &p1, uint8_t &p2, uint8_t &t, uint8_t &n, uint8_t &d);

    void ClockCycles(int cycles);  
    Gallant::Signal1<int> RanCycles;
    
 private:
    Pulse *pulse1_;
    Pulse *pulse2_;
    Triangle *triangle_;
    Noise *noise_;
    DMC *dmc_;
    Channel **channels_;

    // TODO: Implement 5-step mode and IRQ
    uint16_t frame_counter_;  // These are in terms of CPU cycles, not Apu
    void QuarterClock();
    void HalfClock();
};

#endif  // NES_APU_APU_H_
