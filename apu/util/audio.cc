#include "audio.h"
#include <iostream>

// static double wall_time() {
//     struct timeval tv;
//     gettimeofday(&tv, NULL);
//     return (double)tv.tv_sec + (double)tv.tv_usec * 0.000001;
// }

static int paCallback( const void *inputBuffer, void *outputBuffer,
                       unsigned long framesPerBuffer,
                       const PaStreamCallbackTimeInfo* timeInfo,
                       PaStreamCallbackFlags statusFlags,
                       void *userData ) {
    AudioData *ad = (AudioData *)userData;

    queue<vector<uint8_t>> to_mix;

    double play_gap = ad->apu->cpu()->play_gap();

    for(int i = 0; i < framesPerBuffer; i++) {
        double wt = Pa_GetStreamTime(ad->stream);
        if (wt - ad->play_start > play_gap) {
            ad->play_start = wt;
            ad->new_play = false;
        }
        if (!ad->new_play) {
            ad->cycles += CYCLES_PER_SAMPLE;
            if (ad->apu->cpu()->play_nsf(ad->cycles)) {
                ad->new_play = true;
            }
        }
        for(int i = 0; i < CYCLES_PER_SAMPLE; i++) {
            ad->apu->CPUClock();
        }
        vector<uint8_t> *partial = new vector<uint8_t>;
        ad->apu->GetCurrent(*partial);
        to_mix.push(*partial);
    }

    queue<float> mixed;
    APUMixer apu_mixer;

    apu_mixer.Mix(to_mix, SAMPLE_RATE, ad->prev, mixed);
    if(!mixed.empty())
        ad->prev = mixed.back();

    float *out = (float*)outputBuffer;
    unsigned int i;
    (void) inputBuffer; /* Prevent unused variable warning. */

    for( i=0; i<framesPerBuffer && !mixed.empty(); i++ ) {
        *out++ = mixed.front();
        mixed.pop();
    }
    return 0;
}

Audio::Audio(APU *apu) {
    ad_.apu = apu;
    PaError err = Pa_Initialize();
    if( err != paNoError ) 
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );


    int numDevices = Pa_GetDeviceCount();
    const   PaDeviceInfo *deviceInfo;
    for( int i=0; i<numDevices; i++ )
        {
            deviceInfo = Pa_GetDeviceInfo( i );
            cout << deviceInfo->name << endl;
        }



    PaStreamParameters out;
    out.device = 2;
    out.channelCount = 1;
    out.sampleFormat = paFloat32;
    out.suggestedLatency = 0;
    out.hostApiSpecificStreamInfo = NULL;
    /* Open an audio I/O stream. */
    err = Pa_OpenStream( &ad_.stream,
                         NULL,          /* no input channels */
                         &out,          /* mono output */
                         SAMPLE_RATE,
                         paFramesPerBufferUnspecified,
                                        /* frames per buffer, i.e. the number
                                           of sample frames that PortAudio will
                                           request from the callback. Many apps
                                           may want to use
                                           paFramesPerBufferUnspecified, which
                                           tells PortAudio to pick the best,
                                           possibly changing, buffer size.*/
                         0,
                         paCallback, /* this is your callback function */
                         &ad_ );      /*This is a pointer that will be passed to
                                        your callback*/

    ad_.play_start = Pa_GetStreamTime(ad_.stream);

    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );


    err = Pa_StartStream( ad_.stream );
    if( err != paNoError ) 
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );
}

Audio::~Audio() {
    PaError err = Pa_StopStream( ad_.stream );
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );


    err = Pa_CloseStream( ad_.stream );
    if( err != paNoError )
        printf( "PortAudio error: %s\n", Pa_GetErrorText( err ) );

    err = Pa_Terminate();
    if( err != paNoError )
        printf(  "PortAudio error: %s\n", Pa_GetErrorText( err ) );
}
