#ifndef NES_APU_CHANNEL_H_
#define NES_APU_CHANNEL_H_

class Channel {
 public:
    virtual void SetEnabled(bool enabled) = 0;
    virtual void SetByte(uint16_t addr, uint8_t b) = 0;
    virtual void TimerClock() = 0;
    virtual void EnvelopeClock() = 0;
    virtual void LengthClock() = 0;
    virtual uint8_t GetCurrent() = 0;
 protected:
    const uint8_t kLengthTable[32] =
        {10, 254, 20,  2, 40,  4, 80,  6, 160,  8, 60, 10, 14, 12, 26, 14,
         12,  16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30};

    struct Divider {
        uint16_t reload;
        uint16_t counter;
    };
};

#endif  // NES_APU_CHANNEL_H_
