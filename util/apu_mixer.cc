#include "apu_mixer.h"

APUMixer::APUMixer() {
    pulse_lookup.push_back(0);
    for (int i = 1; i < 32; i++) {
        pulse_lookup.push_back(95.52 / (8128.0 / i + 100));
    }
    tnd_lookup.push_back(0);
    for (int i = 1; i < 188; i++) {
        tnd_lookup.push_back(163.67 / (24329.0 / i + 100));
    }
}

// operates on vectors of fixed size
void APUMixer::Mix(const vector<vector<uint8_t>> &data, vector<float> &output) {
    output.clear();
    for (vector<vector<uint8_t>>::const_iterator it = data.cbegin();
         it != data.end(); it++) {
        output.push_back(MixOne(*it));
    }
    Highpass(output, 90);
    Highpass(output, 440);
    Lowpass(output, 14000);
}

void APUMixer::Highpass(vector<float> &data, float frequency) { //first order
    // TODO
}

void APUMixer::Lowpass(vector<float> &data, float frequency) { //first order
    // TODO
}

float APUMixer::MixOne(const vector<uint8_t> s) {
    int p1, p2, t, n, d;
    p1 = s[0]; p2 = s[1]; t = s[2]; n = s[3]; d = s[4];
    float pul = pulse_lookup[p1 + p2];
    float tnd = tnd_lookup[3*t + 2*n + d];
    return pul + tnd;
}
