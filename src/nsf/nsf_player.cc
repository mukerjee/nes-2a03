#include <stdint.h>
#include <stdlib.h>
#include <curses.h>

#include "nsf.h"

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

    play_song(file, song);
/*
    printw("NSF Player:\n");
    printw("%s\n", g_reader->name());
    printw("%s\n", g_reader->artist());
    printw("%s\n", g_reader->copyright_holder());
    printw("Song: %d / %d", song, g_reader->num_songs());
*/

    bool quit = false;
    while(!quit) {
        int c = getch();
        switch(c) {
        case 'q':
            quit = true;
            break;
/*
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
*/
        }
    }

    endwin();
    return 0;
}
