#include "audio_streamer.h"

AudioStreamer::AudioStreamer(AudioSource *as)  {
    PaError err = Pa_Initialize();
    if(err != paNoError) 
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    err = Pa_OpenDefaultStream( &stream_, 0, 1, paFloat32, as->GetSampleRate(),
                                256, PaCallback, as );

    if(err != paNoError)
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    err = Pa_StartStream(stream_);
    if(err != paNoError) 
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));
}

AudioStreamer::~AudioStreamer() {
    Pa_StopStream(stream_);
    Pa_CloseStream(stream_);
    Pa_Terminate();
}

int AudioStreamer::PaCallback( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData ) {
    AudioSource *as = static_cast<AudioSource*>(userData);
    float *out = (float*)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    bzero(out, framesPerBuffer * sizeof(float));

    for(int i = 0; i < framesPerBuffer; i++ ) {
        *out++ = as->GetSample();
    }
    return 0;
}
