#ifndef NES_AUDIO_ADAPTER_AUDIO_ADAPTER_H_
#define NES_AUDIO_ADAPTER_AUDIO_ADAPTER_H_

#include <stdint.h>
#include <math.h>

#include "audio_source.h"
#include "apu.h"

#include "gallant_signal.h"
#include "readerwriterqueue.h"
#include "atomicops.h"

#define HP_90_ENABLED 0
#define HP_440_ENABLED 0
#define LP_14000_ENABLED 0

class Apu;

class AudioAdapter : public AudioSource {
 public:
    AudioAdapter(Apu *apu, int clock_speed, int sample_rate);
    void RanCycles(int cycles);
    float GetSample();
    unsigned int GetSampleRate() { return sample_rate_; }
    Gallant::Signal0<> ReceivedSample;

    void enable_flip() { enabled_ = !enabled_; }
    void pulse1_enable_flip() { pulse1_enabled_ = !pulse1_enabled_; }
    void pulse2_enable_flip() { pulse2_enabled_ = !pulse2_enabled_; }
    void triangle_enable_flip() { triangle_enabled_ = !triangle_enabled_; }
    void noise_enable_flip() { noise_enabled_ = !noise_enabled_; }
    void dmc_enable_flip() { dmc_enabled_ = !dmc_enabled_; }

 private:
    Apu *apu_;
    unsigned int cycles_per_sample_ = 0;
    unsigned int cycles_since_last_output_ = 0;
    unsigned int sample_rate_ = 44100;

    bool enabled_ = true;

    float pulse_lookup[31];
    float tnd_lookup[203]; // This is approximate (within 4% of DMC)
    float hp90_buf_, hp440_buf_, lp14000_buf_, prev_sample_;

    bool pulse1_enabled_ = true;
    bool pulse2_enabled_ = true;
    bool triangle_enabled_ = true;
    bool noise_enabled_ = true;
    bool dmc_enabled_ = true;

    moodycamel::ReaderWriterQueue<uint8_t> pulse1_queue_;
    moodycamel::ReaderWriterQueue<uint8_t> pulse2_queue_;
    moodycamel::ReaderWriterQueue<uint8_t> triangle_queue_;
    moodycamel::ReaderWriterQueue<uint8_t> noise_queue_;
    moodycamel::ReaderWriterQueue<uint8_t> dmc_queue_;
};

#endif  // NES_AUDIO_ADAPTER_AUDIO_ADAPTER_H_
