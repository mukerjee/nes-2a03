#ifndef APU_MIXER_H
#define APU_MIXER_H

#include <vector>

using namespace std;

class APUMixer {
 public:
    APUMixer();
    void Mix(const vector<vector<uint8_t>> &data, vector<float> &output);
    
 private:
    vector<float> pulse_lookup;
    vector<float> tnd_lookup;    // This is approximate (within 4% of DMC)
    
    void Highpass(vector<float> &data, float frequency);
    void Lowpass(vector<float> &data, float frequency);
    float MixOne(const vector<uint8_t> s);
};

#endif
