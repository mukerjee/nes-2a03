#ifndef APU_MIXER_H
#define APU_MIXER_H

#include <vector>
#include <math.h>
#include <queue>

using namespace std;

class APUMixer {
 public:
    APUMixer();
    void Mix(queue<vector<uint8_t>> &data,
             const int sample_rate, float prev, queue<float> &output);
    
 private:
    vector<float> pulse_lookup;
    vector<float> tnd_lookup;    // This is approximate (within 4% of DMC)
    
    float MixOne(const vector<uint8_t> s);
};

#endif
