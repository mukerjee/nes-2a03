#ifndef NES_APU_DMC_H_
#define NES_APU_DMC_H_

#include <stdint.h>

#include "channel.h"

class DMC : public Channel {
 public:
    void SetEnabled(bool enabled);
    void SetByte(uint16_t addr, uint8_t b);

    void TimerClock();
    void EnvelopeClock() {};
    void LengthClock() {};
    uint8_t GetCurrent();

    bool active() {return active_;}
    bool irq() {return irq_;}

 private:
    bool active_ = false;
    bool irq_ = false;
};

#endif  // NES_APU_DMC_H_
