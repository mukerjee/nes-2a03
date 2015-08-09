#ifndef NES_AUDIO_ADAPTER_AUDIO_ADAPTER_H_
#define NES_AUDIO_ADAPTER_AUDIO_ADAPTER_H_

#include <math.h>

#include "gallant_signal.h"
#include "audio_source.h"

using Gallant::Signal1;

class AudioAdapter {
 public:
    AudioAdapter(AudioSource *as, int clock_speed, int sample_rate);
    void ClockCycles(int cycles);
    Signal1<float> OutputSample;

 private:
    AudioSource *audio_source_;
    unsigned int cycles_per_sample_;
    unsigned int cycles_since_last_output_;
};

#endif  // NES_AUDIO_ADAPTER_AUDIO_ADAPTER_H_
