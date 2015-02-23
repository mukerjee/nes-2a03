#include "apu_mixer.h"

void mixer_init() {
    pulse_lookup.push_back(0);
    for (int i = 1; i < 32; i++) {
        pulse_lookup.push_back(95.52 / (8128.0 / i + 100));
    }
    tnd_lookup.push_back(0);
    for (int i = 1; i < 188; i++) {
        tnd_lookup.push_back(163.67 / (24329.0 / i + 100));
    }
}

void highpass(vector<float> &data, float frequency) { //first order
    // TODO
}

void lowpass(vector<float> &data, float frequency) { //first order
    // TODO
}

float mix(const int *s) {
    int p1, p2, t, n, d;
    p1 = s[0]; p2 = s[1]; t = s[2]; n = s[3]; d = s[4];
    float pul = pulse_lookup[p1 + p2];
    float tnd = tnd_lookup[3*t + 2*n + d];
    return pul + tnd;
}
    

// operates on vectors of fixed size
void apu_mixer(const vector<vector<uint8_t>> &data, vector<float> &output) {
    output.clear();
    for (vector<int*>::const_iterator it = data.cbegin();
         it != data.end(); it++) {
        output.push_back(mix(*it));
    }
    highpass(output, 90);
    highpass(output, 440);
    lowpass(output, 14000);
}
