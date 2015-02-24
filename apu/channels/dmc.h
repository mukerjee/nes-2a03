#ifndef DMC_H
#define DMC_H

#include "channel.h"

class DMC : public Channel {
 public:
    DMC() {};
    
    void SetByte(uint16_t addr, uint8_t b);

    uint8_t GetCurrent();

    bool active() {return active_;}
    bool irq() {return irq_;}

 private:
    bool active_ = false;
    bool irq_ = false;

    void Set4010(uint8_t b);
    void Set4011(uint8_t b);
    void Set4012(uint8_t b);
    void Set4013(uint8_t b);
};

#endif
