#include "dmc.h"

void nes_apu::DMC::SetEnabled(bool enabled) {

}

void nes_apu::DMC::SetByte(uint16_t addr, uint8_t b) {
    switch(addr) {
    case 0x4010:

        break;
    case 0x4011:

        break;
    case 0x4012:

        break;
    case 0x4013:

        break;
    }
}

void nes_apu::DMC::TimerClock() {
    
}

uint8_t nes_apu::DMC::GetCurrent() {
    return 0;
}
