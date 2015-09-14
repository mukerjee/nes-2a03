#include <pthread.h>

#include "nes.h"
#include "audio_streamer.h"
#include "nsf_reader.h"
#include "nsf.h"

#define SAMPLE_RATE 44100  // Hz

pthread_t g_run_thread;

Nes *g_nes = NULL;
AudioStreamer *g_as = NULL;
NSFReader *g_reader = NULL;

void *run_nes(void *np) {
    g_nes->Run();
    delete g_reader;
    delete g_as;
    delete g_nes;
    return NULL;
}

void play_song(char *file, int song) {
    if (g_nes) {
        g_nes->Stop();
        pthread_join(g_run_thread, NULL);
    }

    g_reader = new NSFReader(file);
    g_nes = new Nes(SAMPLE_RATE);
    g_as = new AudioStreamer(g_nes->audio_adapter_);

    uint8_t *memory = (uint8_t *)malloc(1<<16 * sizeof(uint8_t));
    size_t size;
    uint16_t starting_address = g_reader->Cart(memory, &size);
    g_nes->LoadCart(memory, starting_address, size);
    free(memory);

    uint8_t **banks = (uint8_t **)calloc(256, sizeof(uint8_t *));
    for(int i = 0; i < 256; i++) {
        banks[i] = (uint8_t *)calloc(4096, sizeof(uint8_t));
    }
    g_reader->Banks(banks);
    for(int i = 0; i < 256; i++) {
        g_nes->LoadBank(banks[i], i, 4096);
        free(banks[i]);
    }
    free(banks);

    g_nes->SetLogging(file, song);

    g_nes->SetRegisters(song, g_reader->is_pal(), 0, 0xFF, 0x5000);
    g_nes->InitSoundRegisters();

    g_nes->RunPeriodic(g_reader->PlayInterval());
    pthread_create(&g_run_thread, NULL, run_nes, NULL);
}
