#ifndef NES_APU_NOISE_H_
#define NES_APU_NOISE_H_

#include <stdint.h>

#include "channel.h"

namespace nes_apu {
    class Noise : public Channel {
    public:
        void SetEnabled(bool enabled);
        void SetByte(uint16_t addr, uint8_t b);

        void TimerClock();
        void EnvelopeClock();
        void LengthClock();
        uint8_t GetCurrent();
    
    private:
        // TODO: Correct?
        const uint16_t kPeriodTable[16] =
            {  4,    8,   16,   32,   64,   96,  128,  160,
             202,  254,  380,  508,  762, 1016, 2034, 4068};
        /* { 0x002, 0x004, 0x008, 0x010, 0x020, 0x030, 0x040, 0x050,  */
        /*   0x065, 0x07F, 0x0BE, 0x0FE, 0x17D, 0x1FC, 0x3F9, 0x7F2 };  */

        bool enabled_ = false;

        Divider timer_ = {.counter = 0, .reload = 0};
        Divider envelope_ = {.counter = 0, .reload = 0};
        uint8_t length_ = 0;

        uint8_t constant_volume_ = 0;
        uint8_t envelope_volume_ = 0;
        bool envelope_start_flag_ = false;

        uint16_t shift_register_ = 1;
        bool mode_flag_ = false;
        bool control_flag_ = false;
    };
}

#endif  // NES_APU_NOISE_H_
