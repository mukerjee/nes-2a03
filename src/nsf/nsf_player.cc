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
    
	printw("\r\r\r\r\rNSF Player:\n");
    printw("%s\n", song_name());
    printw("%s\n", song_artist());
    printw("%s\n", song_copyright_holder());
    printw("Song: %d / %d", song, nsf_num_songs());

    bool quit = false;
    while(!quit) {
        int c = getch();
        switch(c) {
        case 'q':
            quit = true;
            break;
        case '1':
            toggle_pulse1();
            break;
        case '2':
            toggle_pulse2();
            break;
        case '3':
            toggle_triangle();
            break;
        case '4':
            toggle_noise();
            break;
        case '5':
            toggle_dmc();
            break;
        case ' ':
            play_pause();
            break;
        case 'r':
            play_song(file, song);
            break;
        case KEY_LEFT:
            play_song(file, --song);
    		printw("\rSong: %d / %d", song, nsf_num_songs());
            break;
        case KEY_RIGHT:
            play_song(file, ++song);
    		printw("\rSong: %d / %d", song, nsf_num_songs());
            break;
        }
    }

    endwin();
    return 0;
}
