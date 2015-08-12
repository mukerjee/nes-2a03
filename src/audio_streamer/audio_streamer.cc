#include "audio_streamer.h"

AudioStreamer::AudioStreamer(int sample_rate) : samples_(1000) {
    PaError err = Pa_Initialize();
    if(err != paNoError) 
        printf("PortAudio error: %s\n", Pa_GetErrorText(err));

    int numDevices = Pa_GetDeviceCount();
    const PaDeviceInfo *deviceInfo;

    for(int i = 0; i < numDevices; i++ ) {
        deviceInfo = Pa_GetDeviceInfo(i);
        //printf("%s\n", deviceInfo->name);
    }

    PaStreamParameters out;
    out.device = 2; //7;
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
    samples_.enqueue(sample);
}

int AudioStreamer::PaCallback( const void *inputBuffer, void *outputBuffer,
                               unsigned long framesPerBuffer,
                               const PaStreamCallbackTimeInfo* timeInfo,
                               PaStreamCallbackFlags statusFlags,
                               void *userData ) {
    moodycamel::ReaderWriterQueue<float> *samples =
        static_cast<moodycamel::ReaderWriterQueue<float>*>(userData);
    float *out = (float*)outputBuffer;
    (void) inputBuffer; /* Prevent unused variable warning. */

    bzero(out, framesPerBuffer*sizeof(float));

    for(int i = 0; i < framesPerBuffer && samples->peek() != nullptr; i++ ) {
        *out++ = *samples->peek();
        samples->pop();
    }
    return 0;
}
