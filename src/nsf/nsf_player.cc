#include "nes.h"
#include "audio-streamer.h"
#include "nsf.h"

#define SAMPLE_RATE 44100  // Hz

int main(int argc, char **argv) {
    Nes nes(SAMPLE_RATE);
    AudioStreamer as(SAMPLE_RATE);

    nes.audio_adapter_.OutputSample.Connect(&as, &AudioStreamer::AddSample);

    NSFReader reader = NSFReader(argv[1]);

    nes.LoadCart(reader->Cart());

    nes.Run();
    nes.RunPeriodic(reader->play_interval());
    
    sleep(10);
}
