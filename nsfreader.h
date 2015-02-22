#ifndef NSF_READER_H
#define NSF_READER_H

#define MAGIC_STRING "NESM"
#define MAGIC_BYTE '\0x1A'

/* Class for reading and interpreting NSF audio files */
class NSFReader {
 public:
    NSFReader(char *nsffile);
    
    uint8_t nsf_verison_;
    uint8_t num_songs_;
    uint8_t starting_song_;

 private:
    uint16_t data_load_addr_;
    uint16_t data_init_addr_;
    uint16_t data_play_addr_;
    char name_[32];
    char artist_[32];
    char copyright_holder_[32];
};

#endif
