#ifndef AUDIO_STREAMER_AUDIO_STREAMER_H_
#define AUDIO_STREAMER_AUDIO_STREAMER_H_

#include <queue>
#include "portaudio.h"

class AudioStreamer {
 public:
    AudioStreamer(int sample_rate);
    ~AudioStreamer();
    
    void AddSample(float sample);

    static int PaCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData );
 private:
    std::queue<float> samples_;
    PaStream *stream_;
};

#endif  // AUDIO_STREAMER_AUDIO_STREAMER_H_
