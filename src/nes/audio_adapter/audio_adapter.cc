#import "audio_adapter.h"

AudioAdapter::AudioAdapter(AudioSource *as, int clock_speed, int sample_rate) {
    audio_source_ = as;
    cycles_per_sample_ = int(floor(float(clock_speed) / sample_rate));
}

void AudioAdapter::ClockCycles(int cycles) {
    cycles_since_last_output_ += cycles;
    if (cycles_since_last_output_ >= cycles_per_sample_) {
        OutputSample(audio_source_->GetSample());        
        cycles_since_last_output_ -= cycles_per_sample_;
    }
}
