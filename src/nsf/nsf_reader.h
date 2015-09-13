#ifndef NSF_NSF_READER_H_
#define NSF_NSF_READER_H_

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_STRING "NESM"
#define MAGIC_BYTE 0x1A
#define MAX_NUM_BANKS 256
#define BANK_SIZE (1 << 12)

/* Class for reading and interpreting NSF audio files */
class NSFReader {
 public:
    NSFReader(char *nsffile);

    float PlayInterval();
    uint16_t Cart(uint8_t *memory, size_t *size);
    void Banks(uint8_t **banks);

    bool is_pal();
    uint8_t starting_song();

    char *name() { return name_; }
    char *artist() { return artist_; }
    char *copyright_holder() { return copyright_holder_; }
    int num_songs() { return num_songs_; }
    
 private:
    char *file_name_;
    uint8_t nsf_version_;
    uint8_t num_songs_;
    uint8_t starting_song_;
    uint16_t data_load_addr_;
    uint16_t data_init_addr_;
    uint16_t data_play_addr_;
    char name_[32];
    char artist_[32];
    char copyright_holder_[32];
    uint16_t ntsc_speed_;
    uint8_t bankswitch_init_[8];
    uint16_t pal_speed_;
    bool is_pal_;
    bool is_dual_region_;
    uint8_t extra_sound_chips_;
    uint8_t null_expansion_[4];
    uint8_t **banks_;

    bool is_bank_switched();
};

#endif  // NSF_NSF_READER_H_
