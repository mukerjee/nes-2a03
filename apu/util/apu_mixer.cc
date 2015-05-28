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

void APUMixer::Mix(queue<vector<uint8_t>> &data, int sample_rate, float prev, queue<float> &output) {
    vector<float> mixed;
    while(!data.empty()) {
        mixed.push_back(2*MixOne(data.front()) - 1);
        data.pop();
    }

    if(!mixed.empty()) { // filtering
        vector<float> hp1, hp2, out;
        float RC = 1.0 / (2*M_PI * 90);
        float dt = 1.0 / sample_rate;
        float a_hp90 = RC / (RC + dt);

        RC = 1.0 / (2*M_PI * 440);
        float a_hp440 = RC / (RC + dt);

        RC = 1.0 / (2*M_PI * 14000);
        float a_lp14000 = dt / (RC + dt);

        hp1.push_back(prev);
        hp2.push_back(prev);
        out.push_back(prev);

        for(int i = 1; i < mixed.size(); i++) {
            hp1.push_back(a_hp90 *(hp1[i-1] + mixed[i] - mixed[i-1]));
            hp2.push_back(a_hp440 *(hp2[i-1] + hp1[i] - hp1[i-1]));
            out.push_back(out[i-1] + a_lp14000 * (hp2[i] - hp2[i-1]));
        }
        for (vector<float>::iterator it = out.begin(); it != out.end(); ++it) {
            output.push(*it);
        }
    }
}

float APUMixer::MixOne(const vector<uint8_t> s) {
    int p1, p2, t, n, d;
    p1 = s[0]; p2 = s[1]; t = s[2]; n = s[3]; d = s[4];
    float pul = pulse_lookup[p1 + p2];
    float tnd = tnd_lookup[3*t + 2*n + d];
    return pul + tnd;
}
