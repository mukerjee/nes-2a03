#include "nsfreader.h"
#include "apu.h"
#include "apu_mixer.h"
#include "resample.h"

#include <stdio.h>
#include <vector>

#define BYTE_DEPTH 2
#define SAMPLE_RATE 48000
#define OUTPUT_FILE "test.wav"

int main(int argc, char **argv) {
    printf("NSF Player launched  [%s]", argv[1]);

    APU apu();
    NSFReader nsfreader(args.nsffile);
    vector<char*> instr = nsfreader.read_instructions();
    
    // TODO assume instr is a list of instructions
    vector<int*> data = [];
    for (vector<char*>::const_iterator it = instr.begin(); it != istr.end(); ++it) {
        data.push_back(apu.compute(i));
    }

    // TODO should we split up data into chunks before mixing/filtering?
    vector<float> output;
    vector<int16_t> sampled_output;

    mixer_init();
    apu_mixer(data, output);
    resample(output, BYTE_DEPTH, SAMPLE_RATE, sampled_output);
    write_wave(OUTPUT_FILE, BYTE_DEPTH, SAMPLE_RATE, sampled_output);

    return 0;
}
