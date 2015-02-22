#include "apu.h"

void APU::Set4015(uint8_t b) {  // enable individual channels
    pulse1_.SetEnabled(b & 1);
    pulse2_.SetEnabled(b >> 1 & 1);
    triangle_.SetEnabled(b >> 2 & 1);
    noise_.SetEnabled(b >> 3 & 1);
    dmc_.SetEnabled(b >> 4 & 1);
}
 
void APU::Set4017(uint8_t b) {  // frame counter
    fc_five_step_mode_ = b >> 7;
    fc_irq_inhibit_ = b >> 6 & 1;
    if (fc_irq_inhibit_)
        fc_irq_ = false;
    if (fc_five_step_mode_) {
        FCQuarterClock();
        FCHalfClock();
    }
    fc_reset_timer_.set_value(4);
    if (fc_divider_.value())  // middle of APU cycle
        fc_reset_timer_.set_value(3);
} 
 
uint8_t APU::Get4015() {
    p1 = self.pulse1.get_length_counter();
    p2 = self.pulse2.get_length_counter();
    t = self.triangle.get_length_counter();
    n = self.noise.get_length_counter();
    d = self.dmc.get_active();
    i = self.dmc.get_interrupt();
    f = self.frame_counter_interrupt;
    // TODO
    // I don't handle the case where an interrupt happens during a read
    // this should result in the interrupt being read but not having it
    // reset
    self.frame_counter_interrupt = 0;

    return p1 + (p2 << 1) + (t << 2) + (n << 3) + (d << 4) +
        (i << 6) + (f << 7);
}

void APU::FCQuarterClock() {
    if (fc_please_quarter_clock_) {
        pulse1_.EnvelopeClock();
        pulse2_.EnvelopeClock();
        noise_.EnvelopeClock();
        triangle_.LinearCounterClock();
        fc_please_quarter_clock_ = false;
    }
}

void APU::FCHalfClock() {
    if (fc_please_half_clock_) {
        pulse1_.LengthCounterClock();
        pulse2_.LengthCounterClock();
        triangle_.LengthCounterClock();
        noise_.LengthCounterClock();

        pulse1_.SweepClock();
        pulse2_.SweepClock();
        fc_please_half_clock_ = false;
    }
}

void APU::FCReset() {
    fc_sequencer_.set_value(0);
}

void APU::APUClock() {
    fc_sequencer_.set_value(fc_sequencer_.value()+1);
    switch(fc_sequencer_.value()) {
    case 3728:
        fc_please_quarter_clock = true;
    case 7456:
        fc_please_quarter_clock = true;
        fc_please_half_clock_ = true;
    case 11185:
        fc_please_quarter_clock = true;
    case 14914:
        if (fc_five_step_mode_) {
            fc_please_quarter_clock = true;
            fc_please_half_clock_ = true;
            if (!fc_irq_inhibit_)
                fc_counter_interrupt_ = true;
            fc_sequencer_.set_value(-1);
        }
    case 18640:
        fc_please_quarter_clock = true;
        fc_please_half_clock_ = true;
        fc_sequencer_.set_value(-1);
    }

    pulse1_.TimerClock();
    pulse2_.TimerClock();
    noise_.TimeClock();
    dmc_.TimerClock();
}

void APU::CPUClock() {
    fc_reset_timer.Clock();
    FCQuarterClock();
    FCHalfClock();
    fc_divider_.Clock();
    triangle_.TimerClock();
}

void APU::GetCurrent(vector<uint8_t*>& output) {
    output.clear();
    output.push_back(pulse1_.GetCurrent());
    output.push_back(pulse2_.GetCurrent());
    output.push_back(triangle_.GetCurrent());
    output.push_back(noise_.GetCurrent());
    output.push_back(dmc_.GetCurrent());
}


