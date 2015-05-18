#ifndef NSF_READER_H
#define NSF_READER_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAGIC_STRING "NESM"
#define MAGIC_BYTE 0x1A
#define MAX_NUM_BANKS 256
#define BANK_SIZE 1 << 12

/* Class for reading and interpreting NSF audio files */
class NSFReader {
 public:
    NSFReader(char *nsffile);

    bool is_bank_switched();
    bool is_pal();
    bool is_dual_region();
    uint8_t bankswitch_init(int i);
    uint8_t starting_song();
    uint16_t data_load_address();
    uint16_t data_init_address();
    uint16_t data_play_address();
    uint16_t ntsc_speed();
    uint16_t pal_speed();
    uint8_t banks(int i, int addr);
    
 private:
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
    uint8_t pal_ntsc_bits_;
    uint8_t extra_sound_chips_;
    uint8_t null_expansion_[4];
    uint8_t **banks_;
};

#endif
