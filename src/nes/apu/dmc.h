#ifndef NES_APU_DMC_H_
#define NES_APU_DMC_H_

#include <stdint.h>

#include "gallant_signal.h"

#include "channel.h"

#include "nes.h"

class Nes;

class DMC : public Channel {
 public:
    DMC(Nes *nes) {nes_ = nes;};
    
    void SetEnabled(bool enabled);
    void SetByte(uint16_t addr, uint8_t b);

    void TimerClock();
    void EnvelopeClock() {};
    void LengthClock() {};

    uint8_t GetCurrent() {return output_level_;};

    uint8_t bytes_remaining() {return bytes_remaining_.counter;};

    Gallant::Signal0<> IRQ;

 private:
    static const uint8_t kRateLength = 16;
    const uint16_t kRates[kRateLength] =  // CPU Clocks
        {428, 380, 340, 320, 286, 254, 226, 214,
         190, 160, 142, 128, 106,  84,  72,  54};

    Nes *nes_;
    
    bool enabled_ = false;
    
    bool irq_enabled_ = false;
    bool loop_flag_ = false;

    Divider timer_ = {.counter = 0, .reload = 0};
    Divider sample_address_ = {.counter = 0x0000, .reload = 0x0000};
    Divider bytes_remaining_ = {.counter = 0, .reload = 0};

    uint8_t shift_register_ = 0;
    uint8_t shift_register_bits_remaining_ = 0;

    uint8_t output_level_ = 0;
};

#endif  // NES_APU_DMC_H_
