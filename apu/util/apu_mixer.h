#ifndef APU_MIXER_H
#define APU_MIXER_H

#include <vector>
#include <math.h>

#include "resample.h"

using namespace std;

class APUMixer {
 public:
    APUMixer();
    void Mix(const vector<vector<uint8_t>> &data, const int bit_depth, 
             const int sample_rate, vector<int16_t> &output);
    
 private:
    vector<float> pulse_lookup;
    vector<float> tnd_lookup;    // This is approximate (within 4% of DMC)
    
    void Highpass(vector<int16_t> &data, float frequency, const int sample_rate,
                  vector<int16_t> &output);
    void Lowpass(vector<int16_t> &data, float frequency, const int sample_rate, 
                 vector<int16_t> &output);
    float MixOne(const vector<uint8_t> s);
};

#endif
