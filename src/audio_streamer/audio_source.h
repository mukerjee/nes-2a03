#ifndef AUDIO_STREAMER_AUDIO_SOURCE_H_
#define AUDIO_STREAMER_AUDIO_SOURCE_H_

class AudioSource {
 public:
    virtual ~AudioSource() {}
    virtual float GetSample() = 0;
    virtual unsigned int GetSampleRate() = 0;
};

#endif  // AUDIO_STREAMER_AUDIO_SOURCE_H_
