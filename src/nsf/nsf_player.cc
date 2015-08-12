#include <stdint.h>
#include <stdlib.h>

#include "nes.h"
#include "audio_streamer.h"
#include "nsf_reader.h"

#define SAMPLE_RATE 44100  // Hz

int main(int argc, char **argv) {
    Nes nes(SAMPLE_RATE);
    AudioStreamer as(SAMPLE_RATE);

    nes.audio_adapter_.OutputSample.Connect(&as, &AudioStreamer::AddSample);

    NSFReader reader = NSFReader(argv[1]);

    uint8_t *memory = (uint8_t *)malloc(1<<16 * sizeof(uint8_t));
    size_t size;
    uint16_t starting_address = reader.Cart(memory, &size);
    nes.LoadCart(memory, starting_address, size);

    uint8_t **banks = (uint8_t **)calloc(256, sizeof(uint8_t *));
    for(int i = 0; i < 256; i++) {
        banks[i] = (uint8_t *)calloc(4096, sizeof(uint8_t));
    }
    reader.Banks(banks);
    for(int i = 0; i < 256; i++) {
        if (banks[i])
            nes.LoadBank(banks[i], i, 4096);
    }

    int starting_song = reader.starting_song() - 1;
    starting_song = atoi(argv[2]);
    nes.SetRegisters(starting_song, reader.is_pal(), 0, 0xFF, 0x5000);

    nes.RunPeriodic(reader.PlayInterval());
    nes.Run();
}
