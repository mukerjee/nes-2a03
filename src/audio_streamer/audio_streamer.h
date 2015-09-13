#ifndef AUDIO_STREAMER_AUDIO_STREAMER_H_
#define AUDIO_STREAMER_AUDIO_STREAMER_H_

#include <stdio.h>
#include <string.h>

#include "portaudio.h"

#include "audio_source.h"

class AudioStreamer {
 public:
    AudioStreamer(AudioSource *as);
    ~AudioStreamer();
    
    static int PaCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData );
 private:
    PaStream *stream_;
};

#endif  // AUDIO_STREAMER_AUDIO_STREAMER_H_
