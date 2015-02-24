#include "apu.h"
#include "apu_mixer.h"
#include "resample.h"

#include <vector>
#include <iostream>

#define CLOCK_SPEED 1789773  // Hz (NTSC)

#define a4000 16384
#define a4001 16385
#define a4002 16386
#define a4003 16387

#define a4004 16388
#define a4005 16389
#define a4006 16390
#define a4007 16391

#define a4008 16392
#define a4009 16393
#define a400A 16394
#define a400B 16395

#define a400C 16396
#define a400D 16397
#define a400E 16398
#define a400F 16399

#define a4010 16400
#define a4011 16401
#define a4012 16402
#define a4013 16403

#define a4014 16404
#define a4015 16405
#define a4016 16406
#define a4017 16407

void Compute(APU &apu,  vector<vector<uint8_t>> &output) {
    apu.CPUClock();
    vector<uint8_t> *partial = new vector<uint8_t>;
    apu.GetCurrent(*partial);
    output.push_back(*partial);
}

void Set(APU &apu, vector<vector<uint8_t>> &output, int addr,
         uint8_t b) {
    Compute(apu, output);
    if(addr == a4000) apu.Set4000(b);
    if(addr == a4001) apu.Set4001(b);
    if(addr == a4002) apu.Set4002(b);
    if(addr == a4003) apu.Set4003(b);

    if(addr == a4004) apu.Set4004(b);
    if(addr == a4005) apu.Set4005(b);
    if(addr == a4006) apu.Set4006(b);
    if(addr == a4007) apu.Set4007(b);

    if(addr == a4008) apu.Set4008(b);
    if(addr == a400A) apu.Set400A(b);
    if(addr == a400B) apu.Set400B(b);

    if(addr == a400C) apu.Set400C(b);
    if(addr == a400E) apu.Set400E(b);
    if(addr == a400F) apu.Set400F(b);

    if(addr == a4010) apu.Set4010(b);
    if(addr == a4011) apu.Set4011(b);
    if(addr == a4012) apu.Set4012(b);
    if(addr == a4013) apu.Set4013(b);

    if(addr == a4015) apu.Set4015(b);
    if(addr == a4017) apu.Set4017(b);

    Compute(apu, output);
}

void HalfSecondNOP(APU &apu, vector<vector<uint8_t>> &output) {
    for (int i = 0; i < CLOCK_SPEED * 0.5; i++) {
        Compute(apu, output);
    }
}

void TriangleTest(APU &apu, vector<vector<uint8_t>> &output) {
    Set(apu, output, a4015, 4);
    Set(apu, output, a4008, 255);
    Set(apu, output, a400A, 201);
    Set(apu, output, a400B, 250);
    HalfSecondNOP(apu, output);
    HalfSecondNOP(apu, output);
    HalfSecondNOP(apu, output);
    HalfSecondNOP(apu, output);
}

void DutyCycleTest(APU &apu, vector<vector<uint8_t>> &output) {
    Set(apu, output, a4015, 1);
    Set(apu, output, a4000, 191);
    Set(apu, output, a4002, 201);
    Set(apu, output, a4003, 0);
    HalfSecondNOP(apu, output);
    Set(apu, output, a4000, 127);
    HalfSecondNOP(apu, output);
    Set(apu, output, a4000, 63);
    HalfSecondNOP(apu, output);
    Set(apu, output, a4000, 255);
    HalfSecondNOP(apu, output);    
}

void PitchTest(APU &apu, vector<vector<uint8_t>> &output) {
    Set(apu, output, a4015, 1);
    Set(apu, output, a4000, 191);
    Set(apu, output, a4003, 1);
    Set(apu, output, a4002, 0);
    HalfSecondNOP(apu, output);
    Set(apu, output, a4002, 29);
    HalfSecondNOP(apu, output);
    Set(apu, output, a4002, 120);
    HalfSecondNOP(apu, output);
}

int main() {
    APU apu;
    vector<vector<uint8_t>> output;

    printf("start\n");
    TriangleTest(apu, output);
    // DutyCycleTest(apu, output);
    // PitchTest(apu, output);


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
