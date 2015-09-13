#include <stdint.h>
#include <stdlib.h>
#include <pthread.h>
#include <curses.h>

#include "nes.h"
#include "audio_streamer.h"
#include "nsf_reader.h"

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

void reset(char *file, int song) {
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

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("%s nsf_file [starting song] [comparison log]\n", argv[0]);
        return -1;
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    char *file = argv[1];
    
    int song = (argc > 2) ? atoi(argv[2]) : 0;

    reset(file, song);

    printw("NSF Player:\n");
    printw("%s\n", g_reader->name());
    printw("%s\n", g_reader->artist());
    printw("%s\n", g_reader->copyright_holder());
    printw("Song: %d / %d", song, g_reader->num_songs());

    bool quit = false;
    while(!quit) {
        int c = getch();
        switch(c) {
        case 'q':
            quit = true;
            break;
        case '1':
            g_nes->audio_adapter_->pulse1_enable_flip();
            break;
        case '2':
            g_nes->audio_adapter_->pulse2_enable_flip();
            break;
        case '3':
            g_nes->audio_adapter_->triangle_enable_flip();
            break;
        case '4':
            g_nes->audio_adapter_->noise_enable_flip();
            break;
        case '5':
            g_nes->audio_adapter_->dmc_enable_flip();
            break;
        case ' ':
            g_nes->audio_adapter_->enable_flip();
            break;
        case 'r':
            reset(file, song);
            break;
        case KEY_LEFT:
            reset(file, --song);
            printw("\rSong: %d / %d", song, g_reader->num_songs());
            break;
        case KEY_RIGHT:
            reset(file, ++song);
            printw("\rSong: %d / %d", song, g_reader->num_songs());
            break;
        }
    }

    endwin();
    return 0;
}
