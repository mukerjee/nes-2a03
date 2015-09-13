#include "audio_adapter.h"

AudioAdapter::AudioAdapter(Apu *apu, int clock_speed, int sample_rate)
    : apu_(apu), sample_rate_(sample_rate) {
    cycles_per_sample_ = int(floor(float(clock_speed) / sample_rate));

    // Mixing init
    hp90_buf_ = hp440_buf_ = lp14000_buf_ = prev_sample_ = 0.0;
    pulse_lookup[0] = 0.0;
    for (int i = 1; i < 31; i++)
        pulse_lookup[i] = 95.52 / (8128.0 / i + 100);

    tnd_lookup[0] = 0.0;
    for (int i = 1; i < 203; i++)
        tnd_lookup[i] = 163.67 / (24329.0 / i + 100);
}

void AudioAdapter::RanCycles(int cycles) {
    cycles_since_last_output_ += cycles;
    if (cycles_since_last_output_ >= cycles_per_sample_) {
        cycles_since_last_output_ -= cycles_per_sample_;

        uint8_t p1, p2, t, n, d;
        apu_->GetCurrent(p1, p2, t, n, d);

        pulse1_queue_.enqueue(p1);
        pulse2_queue_.enqueue(p2);
        triangle_queue_.enqueue(t);
        noise_queue_.enqueue(n);
        dmc_queue_.enqueue(d);

        ReceivedSample();
    }
}

float AudioAdapter::GetSample() {
    if (!enabled_) {
        return 0;
    }

    uint8_t p1 = pulse1_enabled_ && pulse1_queue_.peek() ?
        *pulse1_queue_.peek() : 0;
    uint8_t p2 = pulse2_enabled_ && pulse2_queue_.peek() ?
        *pulse2_queue_.peek() : 0;
    uint8_t t = triangle_enabled_ && triangle_queue_.peek() ?
        *triangle_queue_.peek() : 0;
    uint8_t n = noise_enabled_ && noise_queue_.peek() ?
        *noise_queue_.peek() : 0;
    uint8_t d = dmc_enabled_ && dmc_queue_.peek() ?
        *dmc_queue_.peek() : 0;
    pulse1_queue_.pop();
    pulse2_queue_.pop();
    triangle_queue_.pop();
    noise_queue_.pop();
    dmc_queue_.pop();

    float pul = pulse_lookup[p1 + p2];
    float tnd = tnd_lookup[3*t + 2*n + d];
    float sample = 2 * (pul + tnd) - 1;


    // filter
    // High pass at 90Hz
    float x = 0.987259; // e ^ (-2 * pi * (90 / 44100))
    float prev_hp90 = hp90_buf_;
    if (HP_90_ENABLED) {
        hp90_buf_ = ((1 + x) / 2) * sample - ((1 + x) / 2) * prev_sample_
            + x * hp90_buf_;
    } else {
        hp90_buf_ = sample;
    }

    // High pass at 440Hz
    x = 0.939235; // e ^ (-2 * pi * (440 / 44100))
    if (HP_440_ENABLED) {
        hp440_buf_ = ((1 + x) / 2) * hp90_buf_ - ((1 + x) / 2) * prev_hp90
            + x * hp440_buf_;
    } else {
        hp440_buf_ = hp90_buf_;
    }

    // Low pass at 14400Hz
    x = 0.136059; // e ^ (-2 * pi * (14000 / 44100))
    if (LP_14000_ENABLED) {
        lp14000_buf_ = (1-x) * hp440_buf_ + x * lp14000_buf_;
    } else {
        lp14000_buf_ = hp440_buf_;
    }

    prev_sample_ = sample;
    return lp14000_buf_;
}
