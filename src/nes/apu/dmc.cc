#include "dmc.h"

void DMC::SetEnabled(bool enabled) {
    enabled_ = enabled;
    if (!enabled_) {
        bytes_remaining_.counter = 0;
    } else if (!bytes_remaining_.counter) {
        sample_address_.counter = sample_address_.reload;
        bytes_remaining_.counter = bytes_remaining_.reload;
    }
}

void DMC::SetByte(uint16_t addr, uint8_t b) {
    switch(addr) {
    case 0x4010:
        irq_enabled_ = b >> 7;
        loop_flag_ = b >> 6;
        timer_.reload = kRates[b & 0xF];
        break;
    case 0x4011:
        output_level_ = b & 0x7F;
        break;
    case 0x4012:
        sample_address_.reload = b << 6 | 0xC000;
        break;
    case 0x4013:
        bytes_remaining_.reload = (b << 4) + 1;
        break;
    }
}

void DMC::TimerClock() {
    if (bytes_remaining_.counter) {
        if (!shift_register_bits_remaining_) {
            shift_register_ = nes_->GetByte(sample_address_.counter);
            sample_address_.counter =
                sample_address_.counter + 1 > 0xFFFF ?
                0x8000 : sample_address_.counter + 1;
            bytes_remaining_.counter--;
            shift_register_bits_remaining_ = 8;
            if (!bytes_remaining_.counter) {
                if (loop_flag_) {
                    sample_address_.counter = sample_address_.reload;
                    bytes_remaining_.counter = bytes_remaining_.reload;
                } else if (irq_enabled_) {
                    IRQ();
                }
            }
        }

        if(!timer_.counter) {
            bool add = shift_register_ & 1;
            if (add && output_level_ <= 125)
                output_level_ += 2;
            else if (!add && output_level_ >= 2)
                output_level_ -= 2;
            shift_register_ >>= 1;
            shift_register_bits_remaining_--;
            timer_.counter = timer_.reload;
        } else timer_.counter--;

    }
}
