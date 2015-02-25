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


// BROKEN (pitches and noise chan?)
void ContinuousTest() {
    for(int j = 0; j < 4; j++) {
        Set(0x4015, 1 << j);
        TimedNOP(0.1);
        Set(0x4000 + 4*j, 0b10111111);
        Set(0x4002 + 4*j, 0b11001001);
        Set(0x4003 + 4*j, 0b00000001);
        TimedNOP(1);
    }
}

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

void PitchTest() {
    for(int j = 0; j < 4; j++) {        
        if (j == 3) { continue; }
        Set(0x4015, 1 << j);
        Set(0x4000 + 4*j, 0b10111111);
        for(int i = 0; i < 0x7FF; i++) {
            Set(0x4003 + 4*j, i >> 8);
            Set(0x4002 + 4*j, i & 0xFF);
            TimedNOP(0.005);
        }
    }
}

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

void EnvelopeTest() {
    for(int j = 0; j < 4; j++) {
        if (j == 2) { continue; }
        Set(0x4015, 1 << j);
        TimedNOP(0.25);
        Set(0x4002 + 4*j, 0b00000000);
        for(int loop = 0; loop < 2; loop++) {
            for(int period = 0; period < 16; period++) {
                Set(0x4000 + 4*j, 0b10000000 + (loop<<5) + period);
                Set(0x4003 + 4*j, 0b00000011);
                TimedNOP(1);
            }
        }
    }
}

void LengthTest() {
    for(int j = 0; j < 4; j++) {
        Set(0x4015, 1 << j);
        TimedNOP(0.25);
        if (j == 2) 
            Set(0x4000 + 4*j, 0b00011111);
        else
            Set(0x4000 + 4*j, 0b10011111);
        Set(0x4002 + 4*j, 0b00000000);
        for(int i = 0; i < 32; i++) {
            Set(0x4003 + 4*j, 0b00000011 + (i<<3));
            TimedNOP(0.25);
        }
    }
}

void SweepTest() {
    for(int j = 0; j < 4; j++) {
        if (j == 2 || j == 3) { continue; }
        Set(0x4015, 1 << j);
        TimedNOP(0.1);
        Set(0x4000 + 4*j, 0b10111111);
        Set(0x4003 + 4*j, 0b00000011);
        Set(0x4002 + 4*j, 0b00000000);
        for(int negate = 0; negate < 2; negate++) {
            for (int period = 0; period < 8; period++) {
                for (int shift = 0; shift < 8; shift++) {
                    Set(0x4001 + 4*j, 0b10000000 + (period << 4) + (negate << 3)
                        + shift);
                    TimedNOP(0.25);
                }
            }
        }
    }
}

void LinearCounterTest() {
    Set(0x4015, 0b00000100);
    Set(0x400A, 0b00000000);
    for (int i = 0; i < 128; i++) {
        Set(0x4008, i);
        Set(0x400B, 0b00000011);
        TimedNOP(0.25);
    }
}

void EdgeCases() {
    for(int j = 0; j < 4; j++) {
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
    Set(0x4015, 3);
    TimedNOP(0.1);

    // BROKEN
    // length counter set small
    Set(0x4000 + 4*2, 0b00011111);
    Set(0x4003 + 4*2, 0b00011011);
    Set(0x4002 + 4*2, 0b00000000);
    TimedNOP(0.1);

    // linear counter set small
    Set(0x4000 + 4*2, 0b00000000);
    Set(0x4003 + 4*2, 0b00011011);
    Set(0x4002 + 4*2, 0b00000000);
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
