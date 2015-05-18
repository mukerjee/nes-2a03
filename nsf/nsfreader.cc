#import "nsfreader.h"

NSFReader::NSFReader(char *nsffile) {
    FILE *fp = fopen(nsffile, "r");
    
    // first 128 bytes are header (http://kevtris.org/nes/nsfspec.txt):
    //   <   little endian   (make sure NSF is really little endian)
    //   4s  4 char string, always NESM
    //   B   one byte, always 1A
    //   B   version number
    //   B   number of songs
    //   B   starting song
    //   H   load address of data
    //   H   init address of data
    //   H   play address of data
    //   32s name of song
    //   32s artist
    //   32s copyright holder
    //   H   speed, in 1/1000000th sec ticks, NTSC
    //   BBBBBBBB  Bankswitch Init Values
    //   H   speed, in 1/1000000th sec ticks, PAL
    //   B   PAL/NTSC bits:
    //         bit 0: if clear, this is an NTSC tune
    //         bit 0: if set, this is a PAL tune
    //         bit 1: if set, this is a dual PAL/NTSC tune
    //         bits 2-7: not used. they *must* be 0
    //   B   Extra Sound Chip Support
    //         bit 0: if set, this song uses VRCVI
    //         bit 1: if set, this song uses VRCVII
    //         bit 2: if set, this song uses FDS Sound
    //         bit 3: if set, this song uses MMC5 audio
    //         bit 4: if set, this song uses Namco 106
    //         bit 5: if set, this song uses Sunsoft FME-07
    //         bits 6,7: future expansion: they *must* be 0
    //   BBBB    for expansion, must be 0
    char header_magic_string[4];
    char header_magic_byte; 
    fread(header_magic_string, sizeof(char), 4, fp);
    fread(&header_magic_byte, sizeof(char), 1, fp);

    fread(&nsf_version_, sizeof(uint8_t), 1, fp);
    fread(&num_songs_, sizeof(uint8_t), 1, fp);
    fread(&starting_song_, sizeof(uint8_t), 1, fp);
    fread(&data_load_addr_, sizeof(uint16_t), 1, fp);
    fread(&data_init_addr_, sizeof(uint16_t), 1, fp);
    fread(&data_play_addr_, sizeof(uint16_t), 1, fp);
    fread(&name_, sizeof(char), 32, fp);
    fread(&artist_, sizeof(char), 32, fp);
    fread(&copyright_holder_, sizeof(char), 32, fp);

    fread(&ntsc_speed_, sizeof(uint16_t), 1, fp);
    fread(&bankswitch_init_, sizeof(uint8_t), 8, fp);
    fread(&pal_speed_, sizeof(uint16_t), 1, fp);
    fread(&pal_ntsc_bits_, sizeof(uint8_t), 1, fp);
    fread(&extra_sound_chips_, sizeof(uint8_t), 1, fp);
    fread(&null_expansion_, sizeof(uint8_t), 4, fp);

    // sanity checks
    if(strncmp(header_magic_string, MAGIC_STRING, 4))        
        printf("NSF header did not begin with \"NESM\"");
    if(header_magic_byte != MAGIC_BYTE)
        printf("Unexpected value in 5th byte of NSF header");
    if(nsf_version_ != 0x01)
        printf("NSF Version not 1\n");
    if(num_songs_ == 0)
        printf("No songs in this file...\n");
    if(starting_song_ == 0 || starting_song_ > num_songs_)
        printf("Bad starting song value\n");
    if(data_load_addr_ < 0x8000)
        printf("Load addr too low\n");
    if(data_init_addr_ < data_load_addr_)
        printf("Init addr too low\n");
    if(data_play_addr_ < data_load_addr_)
        printf("Play addr too low\n");
    if((pal_ntsc_bits_ & 0xFC) != 0)
        printf("pal/ntsc unused bits set?\n");
    for(int i = 0; i < 4; i++) {
        if(null_expansion_[i])
            printf("expansion bits set?\n");
    }

    // read data
    if (is_bank_switched()) {
        banks_ = (uint8_t **)malloc(MAX_NUM_BANKS*sizeof(uint8_t *));
        for(int i = 0; !feof(fp); i++) {
            // 4k banks
            banks_[i] = (uint8_t *)calloc(1, BANK_SIZE * sizeof(uint8_t));
            fread(banks_[i], 1, BANK_SIZE, fp);
        }
    } else {
        banks_ = (uint8_t **)malloc(1*sizeof(uint8_t *));
        banks_[0] = (uint8_t *)calloc(1, 2<<16 * sizeof(uint8_t));
        fread(banks_[0], 1, 2<<16, fp);
    }

    fclose(fp);
}

bool NSFReader::is_bank_switched() {
    bool bank_switched = false;
    for(int i = 0; i < 8; i++) {
        if(bankswitch_init_[i])
            bank_switched = true;
    }
    return bank_switched;
}

bool NSFReader::is_pal() {
    return pal_ntsc_bits_ & 0x01;
}

bool NSFReader::is_dual_region() {
    return pal_ntsc_bits_ & 0x02;
}

uint8_t NSFReader::bankswitch_init(int i) {
    return bankswitch_init_[i];
}

uint8_t NSFReader::starting_song() {
    return starting_song_;
}

uint16_t NSFReader::data_load_address() {
    return data_load_addr_;
}

uint16_t NSFReader::data_init_address() {
    return data_init_addr_;
}

uint16_t NSFReader::data_play_address() {
    return data_play_addr_;
}

uint16_t NSFReader::ntsc_speed() {
    return ntsc_speed_;
}

uint16_t NSFReader::pal_speed() {
    return pal_speed_;
}

uint8_t NSFReader::banks(int i, int addr) {
    return banks_[i][addr];
}
