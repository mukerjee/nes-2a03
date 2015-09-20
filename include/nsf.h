#ifndef NSFLIB_H
#define NSFLIB_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 * PLAYBACK CONTROLS
 */
void play_song(char *file, int song);
void play_pause();
void toggle_pulse1();
void toggle_pulse2();
void toggle_triangle();
void toggle_noise();
void toggle_dmc();


/*
 * SONG INFORMATION
 */
char *song_name();
char *song_artist();
char *song_copyright_holder();


/*
 * NSF INFORMATION
 */
int nsf_num_songs();


#ifdef __cplusplus
}
#endif


#endif /* NSFLIB_H */
