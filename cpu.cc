#include "apu.h"
#include "apu_mixer.h"
#include "resample.h"

#include <vector>
#include <iostream>

#define CLOCK_SPEED 1789773  // Hz (NTSC)

void Compute(APU &apu,  vector<vector<uint8_t>> &output) {
    apu.CPUClock();
    vector<uint8_t> *partial = new vector<uint8_t>;
    apu.GetCurrent(*partial);
    output.push_back(*partial);
}

void Set(APU &apu, vector<vector<uint8_t>> &output, int addr,
         uint8_t b) {
    Compute(apu, output);
    apu.SetByte(addr, b);
    Compute(apu, output);
}

void TimedNOP(float time, APU &apu, vector<vector<uint8_t>> &output) {
    for (int i = 0; i < CLOCK_SPEED * time; i++) {
        Compute(apu, output);
    }
}

void TriangleTest(APU &apu, vector<vector<uint8_t>> &output) {
    Set(apu, output, 0x4015, 0b00000100);
    Set(apu, output, 0x4008, 0b11111111);
    Set(apu, output, 0x400A, 0b11001001);
    Set(apu, output, 0x400B, 0b11111010);
    TimedNOP(2, apu, output);
}

void DutyCycleTest(APU &apu, vector<vector<uint8_t>> &output) {
    Set(apu, output, 0x4015, 0b00000001);
    Set(apu, output, 0x4000, 0b10111111);
    Set(apu, output, 0x4002, 0b11001001);
    Set(apu, output, 0x4003, 0b00000000);
    TimedNOP(0.5, apu, output);
    Set(apu, output, 0x4000, 0b01111111);
    TimedNOP(0.5, apu, output);
    Set(apu, output, 0x4000, 0b00111111);
    TimedNOP(0.5, apu, output);
    Set(apu, output, 0x4000, 0b11111111);
    TimedNOP(0.5, apu, output);    
}

void PitchTest(APU &apu, vector<vector<uint8_t>> &output) {
    Set(apu, output, 0x4015, 0b00000001);
    Set(apu, output, 0x4000, 0b10111111);
    Set(apu, output, 0x4003, 0b00000001);
    Set(apu, output, 0x4002, 0b00000000);
    TimedNOP(0.5, apu, output);
    Set(apu, output, 0x4002, 0b00011101);
    TimedNOP(0.5, apu, output);
    Set(apu, output, 0x4002, 0b01111000);
    TimedNOP(0.5, apu, output);
}

void VolumeTest(APU &apu, vector<vector<uint8_t>> &output) {
    Set(apu, output, 0x4015, 0b00000001);
    Set(apu, output, 0x4003, 0b00000001);
    Set(apu, output, 0x4002, 0b00000000);
    for(int i = 0; i < 16; i++) {
        Set(apu, output, 0x4000, 0b1011 + i);
    }
    TimedNOP(0.5, apu, output);
}

int main() {
    APU apu;
    vector<vector<uint8_t>> output;

    printf("start\n");
    TriangleTest(apu, output);
    DutyCycleTest(apu, output);
    PitchTest(apu, output);


    printf("done instructions, %lu\n", output.size());
    for(vector<vector<uint8_t>>::iterator i = output.begin(); i != output.end(); ++i) {
        //cout << (int)(*i)[2] << ' ';
    }

    APUMixer apu_mixer;
    vector<float> mixed;
    apu_mixer.Mix(output, mixed);
    printf("done mixer, %lu\n", mixed.size());
    for(vector<float>::const_iterator i = mixed.begin(); i != mixed.end(); ++i) {
        //cout << *i << ' ';
    }
    
    
    vector<int16_t> sampled;
    resample(mixed, 16, 48000, sampled);
    printf("done resample %lu\n", sampled.size());
    for(vector<int16_t>::const_iterator i = sampled.begin(); i != sampled.end(); ++i) {
        //cout << *i << ' ';
    }

    write_wave("test.wav", 16, 48000, sampled);
    printf("done wave %lu\n", sampled.size());

    return 0;
}
