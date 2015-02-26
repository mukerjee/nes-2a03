#include "apu.h"
#include "apu_mixer.h"
#include "resample.h"

#include <vector>
#include <iostream>

#define CLOCK_SPEED 1789773  // Hz (NTSC)
#define SAMPLE_RATE 48000 // Hz

#define STEPS_PER_SAMPLE float(CLOCK_SPEED) / SAMPLE_RATE

APU g_apu;
vector<vector<uint8_t>> g_output;
float g_step = 0;

void Compute() {
    g_apu.CPUClock();
    g_step++;
    if (g_step >= floor(STEPS_PER_SAMPLE)) {
        vector<uint8_t> *partial = new vector<uint8_t>;
        g_apu.GetCurrent(*partial);
        g_output.push_back(*partial);
        g_step -= STEPS_PER_SAMPLE;
    }
}

void Set(int addr, uint8_t b) {
    Compute();
    g_apu.SetByte(addr, b);
    Compute();
}

void TimedNOP(float time) {
    for (int i = 0; i < CLOCK_SPEED * time; i++)
        Compute();
}

// BROKEN does the noise channel work right?
// play one note for one second for each channel
void ContinuousTest() {
    for(int j = 0; j < 4; j++) {
        Set(0x4015, 1 << j);
        TimedNOP(0.1);
        Set(0x4000 + 4*j, 0b00111111);
        Set(0x4003 + 4*j, 0b00000000);
        Set(0x4002 + 4*j, 0b11001001);
        TimedNOP(5);
    }
}

// cycle through different duty cycles (12.5, 25, 50, 75) for pulses
void DutyCycleTest() {
    for(int j = 0; j < 4; j++) {
        if (j == 2 || j == 3) { continue; }
        Set(0x4015, 1 << j);
        TimedNOP(0.25);
        Set(0x4002 + 4*j, 0b11001001);
        Set(0x4003 + 4*j, 0b00000000);
        for(int i = 0; i < 4; i++) {
            Set(0x4000 + 4*j, 0b00111111 + (i << 6));
            TimedNOP(0.25);
        }
    }
}

// BROKEN has weird overtones in pulse
// cycle through all period values for pulses and triangle timers. 
// should decrease pitch throughout
void PitchTest() {
    for(int j = 0; j < 4; j++) {        
        if (j == 3) { continue; }
        Set(0x4015, 1 << j);
        Set(0x4000 + 4*j, 0b10111111);
        Set(0x4001 + 4*j, 0b00001000);
        for(int i = 0; i < 0x7FF; i++) {
            Set(0x4003 + 4*j, i >> 8);
            Set(0x4002 + 4*j, i & 0xFF);
            TimedNOP(0.005);
        }
    }
}

// increase volume then decrease volume for pulses and noise
void VolumeTest() {
    for(int j = 0; j < 4; j++) {
        if (j == 2) { continue; }
        Set(0x4015, 1 << j);
        TimedNOP(0.25);
        Set(0x4003 + 4*j, 0b00000001);
        Set(0x4002 + 4*j, 0b00000000);
        for(int i = 0; i < 16; i++) {
            Set(0x4000 + 4*j, 0b10110000 + i);
            TimedNOP(0.15);
        }
        for(int i = 15; i >= 0; i--) {
            Set(0x4000 + 4*j, 0b10110000 + i);
            TimedNOP(0.15);
        }
    }
}

// cycle through envelope length values (then same with loops) for all waves
// should get longer throughout the test (until we test loops)
 void EnvelopeTest() {
    for(int j = 0; j < 4; j++) {
        if (j == 2) { continue; }
        Set(0x4015, 1 << j);
        TimedNOP(0.25);
        Set(0x4002 + 4*j, 0b00000000);
        for(int loop = 0; loop < 2; loop++) {
            for(int period = 0; period < 16; period++) {
                Set(0x4000 + 4*j, 0b10000000 + (loop<<5) + period);
                Set(0x4003 + 4*j, 0b00001011);
                TimedNOP(1);
            }
        }
    }
}

// cycle through length counter values for all channels. 
// should get longer throughout the test. We also do a halt test where notes
// should just stay held.
void LengthTest() {
    uint8_t sorted_lengths[32] = {0x03, 0x05, 0x07, 0x09, 0x0B, 0x00, 0x0D, 0x10, 
                                  0x0F, 0x0C, 0x11, 0x1C, 0x13, 0x15, 0x02, 0x17, 
                                  0x19, 0x12, 0x1B, 0x0E, 0x1D, 0x1F, 0x1E, 0x04, 
                                  0x14, 0x0A, 0x1A, 0x06, 0x16, 0x08, 0x18, 0x01};
    for(int j = 0; j < 4; j++) {
        Set(0x4015, 1 << j);
        TimedNOP(0.25);

        for (int halt = 0; halt < 2; halt++) {
            if (j == 2) 
                Set(0x4000 + 4*j, 0b01111111 + (halt << 7));
            else
                Set(0x4000 + 4*j, 0b10011111 + (halt << 5));
            Set(0x4002 + 4*j, 0b00000000);
            for(int i = 0; i < 32; i++) {
                Set(0x4003 + 4*j, 0b00000011 + (sorted_lengths[i]<<3));
                TimedNOP(2);
            }
        }
    }
}

// cycle through shift, period, and negate values for
// the sweep units in the pulse waves. should sweep down stronger,
// then slower, then sweep up.
void SweepTest() {
    for(int j = 0; j < 4; j++) {
        if (j == 2 || j == 3) { continue; }
        Set(0x4015, 1 << j);
        TimedNOP(0.1);
        Set(0x4000 + 4*j, 0b10111111);
        for(int negate = 0; negate < 2; negate++) {
            for (int period = 1; period < 8; period++) {
                for (int shift = 7; shift > 0; shift--) {
                    Set(0x4003 + 4*j, 0b00000001);
                    Set(0x4002 + 4*j, 0b00001001);
                    Set(0x4001 + 4*j, 0b10000000 + (period << 4) + (negate << 3)
                        + shift);
                    TimedNOP(1);
                }
            }
        }
    }
}

// cycle through possible triangle linear counter values
// notes should become longer throughout the test.
void LinearCounterTest() {
    Set(0x4015, 0b00000100);
    Set(0x400A, 0b00000000);
    for (int i = 0; i < 128; i++) {
        Set(0x400B, 0b00001011);
        Set(0x4008, i);
        TimedNOP(1);
    }
}

// cycles through the 16 periods and 2 modes of noise channel
void NoiseTest() {
    Set(0x4015, 0b00001000);
    Set(0x400C, 0b00111111);
    Set(0x400F, 0b00000000);
    for (int mode = 0; mode < 2; mode++) {
        for (int period = 0; period < 16; period++) {
            Set(0x400E, (mode << 7) + period);
            TimedNOP(0.25);
        }
    }
}

// cycles through the cases where the channels should be muted
// since we can't set the length counter directly, we use a short
// length counter value as a proxy.
void EdgeCases() {
    for(int j = 0; j < 4; j++) {
        continue;
        if (j == 2 || j == 3) { continue; }
        
        // pulse waves
        Set(0x4015, 1 << j);
        TimedNOP(0.1);

        // timer less than 8
        Set(0x4000 + 4*j, 0b10111111);
        for (int i = 7; i >= 0; i--) {
            Set(0x4003 + 4*j, 0b00000000);
            Set(0x4002 + 4*j, i);
            TimedNOP(0.1);
        }

        // length counter set small
        Set(0x4000 + 4*j, 0b10011111);
        Set(0x4003 + 4*j, 0b00011011);
        Set(0x4002 + 4*j, 0b00000000);
        TimedNOP(0.1);

        // sweep output exceeds 7FF
        Set(0x4000 + 4*j, 0b10111111);
        for (int i = 0x400; i < 0x800; i++) {
            Set(0x4003 + 4*j, i >> 8);
            Set(0x4002 + 4*j, i & 255);
            TimedNOP(0.1);
        }
    }

    // triangle
    Set(0x4015, 0b00000100);
    TimedNOP(0.1);

    // length counter set small
    Set(0x4000 + 4*2, 0b00011111);
    Set(0x4003 + 4*2, 0b00011011);
    Set(0x4002 + 4*2, 0b00000000);
    TimedNOP(0.1);

    // linear counter set to 0
    Set(0x4000 + 4*2, 0b00000000);
    Set(0x4003 + 4*2, 0b00011011);
    Set(0x4002 + 4*2, 0b00000000);
    TimedNOP(0.1);
    
    // noise
    Set(0x4015, 0b00001000);
    TimedNOP(0.1);
    
    // length counter set small
    Set(0x4000 + 4*3, 0b00011111);
    Set(0x4003 + 4*3, 0b00011011);
    Set(0x4002 + 4*3, 0b00000000);
    TimedNOP(0.1);
}

int main(int argc, char **argv) {
    if(argc < 2) {
        printf("usage: %s [test]\n", argv[0]);
        return -1;
    }

    printf("start\n");    
    if(!strcmp(argv[1], "continuous"))
        ContinuousTest();
    else if(!strcmp(argv[1], "duty"))
        DutyCycleTest();
    else if(!strcmp(argv[1], "pitch"))
        PitchTest();
    else if(!strcmp(argv[1], "volume"))
        VolumeTest();
    else if(!strcmp(argv[1], "envelope"))
        EnvelopeTest();
    else if(!strcmp(argv[1], "length"))
        LengthTest();
    else if(!strcmp(argv[1], "sweep"))
        SweepTest();
    else if(!strcmp(argv[1], "linear"))
        LinearCounterTest();
    else if(!strcmp(argv[1], "noise"))
        NoiseTest();
    else if(!strcmp(argv[1], "edge"))
        EdgeCases();
    else {
        printf("test %s not found\n", argv[1]);
        return -1;
    }

    printf("done instructions, %lu\n", g_output.size());
    for(vector<vector<uint8_t>>::iterator i = g_output.begin(); i != g_output.end(); ++i) {
        //cout << (int)(*i)[2] << ' ';
    }

    APUMixer apu_mixer;
    vector<float> mixed;
    apu_mixer.Mix(g_output, mixed);
    printf("done mixer, %lu\n", mixed.size());
    for(vector<float>::const_iterator i = mixed.begin(); i != mixed.end(); ++i) {
        //cout << *i << ' ';
    }
        
    vector<int16_t> sampled;
    map_samples(mixed, 16, 48000, sampled);
    printf("done resample %lu\n", sampled.size());
    for(vector<int16_t>::const_iterator i = sampled.begin(); i != sampled.end(); ++i) {
        //cout << *i << ' ';
    }

    write_wave("test.wav", 16, 48000, sampled);
    printf("done wave %lu\n", sampled.size());

    return 0;
}
