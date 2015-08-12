#include "nsf_reader.h"

NSFReader::NSFReader(char *nsffile) {
    file_name_ = nsffile;
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

    uint8_t pal_ntsc_bits;
    fread(&pal_ntsc_bits, sizeof(uint8_t), 1, fp);
    is_pal_ = pal_ntsc_bits & 0x01;
    is_dual_region_ = pal_ntsc_bits & 0x02;

    
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
    if((pal_ntsc_bits & 0xFC) != 0)
        printf("pal/ntsc unused bits set?\n");
    for(int i = 0; i < 4; i++) {
        if(null_expansion_[i])
            printf("expansion bits set?\n");
    }

    // read data
    banks_ = (uint8_t **)calloc(MAX_NUM_BANKS, sizeof(uint8_t *));
    if (is_bank_switched()) {
        uint16_t padding = data_load_addr_ & 0x0FFF;
        for(int i = 0; !feof(fp); i++) {
            // 4k banks
            banks_[i] = (uint8_t *)calloc(BANK_SIZE, sizeof(uint8_t));
            fread(&banks_[i][padding], 1, BANK_SIZE - padding, fp);
            padding = 0;
        }
    } else {
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
    return is_pal_;
}

uint8_t NSFReader::starting_song() {
    return starting_song_;
}


float NSFReader::PlayInterval() {
    // call play ever 1,000,000 / period times a second
    float rate = 1000000.0;
    rate /= is_pal_ ? pal_speed_ : ntsc_speed_;
    return 1.0 / rate;
}

/**
* @brief Creates the cartridge data for the NSF.
*
* @param memory Buffer to write the cart into.
* @param size Size of the output
*
* @return Starting address
*/
uint16_t NSFReader::Cart(uint8_t *memory, size_t *size) {
    if (is_bank_switched()) {
        for (int i = 0; i < 8; i++) {
            memcpy(&memory[0x8000 + i*4096], banks_[bankswitch_init_[i]], 4096);
        }
    }
    else {
        // TODO: the length here looks segfaulty.
        memcpy(&memory[data_load_addr_], banks_[0], 0xFFFF - data_load_addr_);
    }
    
    memory[0x5000] = 0x20; // jsr
    memory[0x5001] = data_init_addr_ & 0xFF;
    memory[0x5002] = data_init_addr_ >> 8;
    memory[0x5003] = 0xF2; // stp

    memory[0x5004] = 0x20; // jsr
    memory[0x5005] = data_play_addr_ & 0xFF;
    memory[0x5006] = data_play_addr_ >> 8;
    memory[0x5007] = 0x4C; // jmp 0x5003
    memory[0x5008] = 0x03;
    memory[0x5009] = 0x50;

    *size = 1 << 16;
    return 0;
}

/**
* @brief Copys the bank data to the input buffer.
*
* @param banks Buffer to store the banks.
*/
void NSFReader::Banks(uint8_t **banks) {
    for(int i = 0; i < MAX_NUM_BANKS; i++) {
        if (banks_[i]) {
            memcpy(banks[i], banks_[i], 4096);
        }
    }    
}
