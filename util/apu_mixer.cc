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
void APUMixer::Mix(const vector<vector<uint8_t>> &data, int bit_depth, int sample_rate, vector<int16_t> &output) {
    vector<float> mixed;
    for (vector<vector<uint8_t>>::const_iterator it = data.cbegin();
         it != data.end(); it++) {
        mixed.push_back(MixOne(*it));
    }
    vector<int16_t> mapped;
    map_samples(mixed, bit_depth, sample_rate, mapped);
    vector<int16_t> hp1, hp2;
    Highpass(mapped, 90, sample_rate, hp1);
    Highpass(hp1, 440, sample_rate, hp2);

    output.clear();
    Lowpass(hp1, 14000, sample_rate, output);
}

void APUMixer::Highpass(vector<int16_t> &data, float frequency, int sample_rate, vector<int16_t> &output) { //first order
    float RC = 1.0 / (2*M_PI * frequency);
    float dt = 1.0 / sample_rate;
    float a = RC / (RC + dt);
    output.push_back(data[0]);
    for(int i = 1; i < data.size(); i++) {
        output.push_back(a *(output[i-1] + data[i] - data[i-1]));
    }
}

void APUMixer::Lowpass(vector<int16_t> &data, float frequency, int sample_rate, vector<int16_t> &output) { //first order
    float RC = 1.0 / (2*M_PI * frequency);
    float dt = 1.0 / sample_rate;
    float a = dt / (RC + dt);
    output.push_back(data[0]);
    for(int i = 1; i < data.size(); i++) {
        output.push_back(output[i-1] + a * (data[i] - output[i-1]));
    }
}

float APUMixer::MixOne(const vector<uint8_t> s) {
    int p1, p2, t, n, d;
    p1 = s[0]; p2 = s[1]; t = s[2]; n = s[3]; d = s[4];
    float pul = pulse_lookup[p1 + p2];
    float tnd = tnd_lookup[3*t + 2*n + d];
    return pul + tnd;
}
