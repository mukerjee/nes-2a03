#ifndef AUDIO_H
#define AUDIO_H

#include <queue>
#include <vector>
#include <mutex>
//#include <sys/time.h>
#include "apu.h"
#include "portaudio.h"
#include "apu_mixer.h"

#define CLOCK_SPEED 1789773  // Hz (NTSC)
#define SAMPLE_RATE 44100 // Hz
#define CYCLES_PER_SAMPLE int(floor(float(CLOCK_SPEED) / SAMPLE_RATE))

using namespace std;

class APU;

static int paCallback( const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData );

struct AudioData {
    float prev = 0;
    int cycles = 0;
    APU *apu;
    double play_start;
    bool new_play = true;
    PaStream *stream;
};

class Audio {
 public:
    Audio(APU *apu);
    ~Audio();

 private:
    AudioData ad_;
};

#endif
