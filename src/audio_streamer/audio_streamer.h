#ifndef AUDIO_STREAMER_AUDIO_STREAMER_H_
#define AUDIO_STREAMER_AUDIO_STREAMER_H_

#include <queue>
#include <strings.h>

#include "portaudio.h"

#include "readerwriterqueue.h"
#include "atomicops.h"

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
    moodycamel::ReaderWriterQueue<float> samples_;
    PaStream *stream_;
};

#endif  // AUDIO_STREAMER_AUDIO_STREAMER_H_
