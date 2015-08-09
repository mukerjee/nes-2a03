#include "audio_streamer.h"

AudioStreamer::AudioStreamer(int sample_rate) {
    PaError err = Pa_Initialize();
    if(err != paNoError) 
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    int numDevices = Pa_GetDeviceCount();
    const PaDeviceInfo *deviceInfo;

    for(int i = 0; i < numDevices; i++ ) {
        deviceInfo = Pa_GetDeviceInfo(i);
        printf("%s", deviceInfo->name);
        printf("%s", "\n");
    }

    PaStreamParameters out;
    out.device = 6;
    out.channelCount = 1;
    out.sampleFormat = paFloat32;
    out.suggestedLatency = 0;
    out.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(&stream_, NULL, &out, sample_rate,
                        paFramesPerBufferUnspecified,
                        0, PaCallback, &samples_);

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

void AudioStreamer::AddSample(float sample) {
    samples_.push(sample);
}

int AudioStreamer::PaCallback( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData ) {
    std::queue<float> *samples = static_cast<std::queue<float> *>(userData);
    float *out = (float*)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    for(int i = 0; i < framesPerBuffer && !samples->empty(); i++ ) {
        *out++ = samples->front();
        samples->pop();
    }
    return 0;
}
