#include "apu.h"
#include "apu_mixer.h"
#include "resample.h"

#include <vector>

void Compute(APU &apu,  vector<vector<uint8_t>> &output) {
    apu.CPUClock();
    vector<uint8_t> *partial = new vector<uint8_t>;
    apu.GetCurrent(*partial);
    output.push_back(*partial);
}

int main() {
    APU apu;

    // instr = ['lda #%00000001',
    //          'sta $4015',
    //          'lda #%10111111',
    //          'sta $4000',
    //          'lda #$C9',
    //          'sta $4002',
    //          'lda #$00',
    //          'sta $4003']

    // instr += ['nop']*1789773*5

    vector<vector<uint8_t>> output;

    printf("start\n");    
    Compute(apu, output);
    apu.Set4015(1);
    Compute(apu, output);
    Compute(apu, output);
    apu.Set4000(191);
    Compute(apu, output);
    Compute(apu, output);
    apu.Set4002(201);
    Compute(apu, output);
    Compute(apu, output);
    apu.Set4003(0);
    Compute(apu, output);
    for (int i = 0; i < 1789773 * 5; i++) {
        Compute(apu, output);
    }
    printf("done instructions\n");

    
    mixer_init();
    vector<float> mixed;
    apu_mixer(output, mixed);
    printf("done mixer\n");
    
    vector<int16_t> sampled;
    resample(mixed, 2, 48000, sampled);
    printf("done resample\n");

    write_wave("test.wav", 2, 48000, sampled);
    printf("done wave\n");

    return 0;
}
