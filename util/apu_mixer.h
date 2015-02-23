#ifndef APU_MIXER_H
#define APU_MIXER_H

#include <vector>

using namespace std;

vector<float> pulse_lookup;

// This is approximate (within 4% of DMC)
vector<float> tnd_lookup;

void mixer_init();
void apu_mixer(const vector<vector<uint8_t>> &data, vector<float> &output);

#endif
