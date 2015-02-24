#include "dmc.h"

void DMC::SetByte(uint16_t addr, uint8_t b) {
    if (addr == 0x4010)
        Set4010(b);
    if (addr == 0x4011)
        Set4011(b);
    if (addr == 0x4012)
        Set4012(b);
    if (addr == 0x4013)
        Set4013(b);
}

void DMC::Set4010(uint8_t b) {

}

void DMC::Set4011(uint8_t b) {

}

void DMC::Set4012(uint8_t b) {

}

void DMC::Set4013(uint8_t b) {

}

uint8_t DMC::GetCurrent() {
    return 0;
}
