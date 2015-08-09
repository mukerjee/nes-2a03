#ifndef NES_AUDIO_ADAPTER_AUDIO_SOURCE_H_
#define NES_AUDIO_ADAPTER_AUDIO_SOURCE_H_

class AudioSource {
 public:
    virtual ~AudioSource() {}
    virtual float GetSample() = 0;
};

#endif  // NES_AUDIO_ADAPTER_AUDIO_SOURCE_H_
