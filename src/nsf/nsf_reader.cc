#import "nsfreader.h"

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

char *NSFReader::file_name() {
    return file_name_;
}






/*************** EXECUTION ***************/
/**
* @brief init nsf (music file) player
*/
void NesCpu::init_nsf(NSFReader *reader) {
    reader_ = reader;

    // zero memory
    for(int i = 0; i < 0x0800; i++)
        memory_->set_byte(i, 0);
    for(int i = 0x6000; i < 0x8000; i++)
        memory_->set_byte(i, 0);
    
    // initialize audio
    for(int i = 0x4000; i < 0x4014; i++)
        memory_->set_byte(i, 0);
    memory_->set_byte(0x4010, 0x10);
    memory_->set_byte(0x4015, 0x0F);

    // set frame counter to 4-step mode
    memory_->set_byte(0x4017, 0x40);

    // initialize banks
    if (reader_->is_bank_switched()) {
        for(int i = 0; i < 8; i++) {
            memory_->set_byte(0x5FF8 + i, reader_->bankswitch_init(i));
        }
    }
    
    // set starting song
    track_ = reader_->starting_song() - 1;
    register_a_ = track_;

    // determine if pal or ntsc
    register_x_ = reader_->is_pal();

    // load program
    if (reader_->is_bank_switched()) { // bank switching
        uint16_t padding = reader_->data_load_address() & 0x0FFF;
        uint16_t start_address = 0x8000 + padding;
        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < (1 << 12); j++) {
                memory_->set_byte(start_address + (i * 1<<12) + j,
                                  reader_->banks(i, j));
            }
        }
    } else { // no bank switching
        for(int i = reader_->data_load_address(); i < 0xFFFF; i++) {
            memory_->set_byte(i, reader_->banks(0, i - reader_->data_load_address()));
        }
    }
    
    interrupt_disable_ = true;

    register_s_ = 0xFF;

    memory_->set_byte(0x5000, 0x20); // jsr
    memory_->set_byte(0x5001, reader_->data_init_address() & 0xFF);
    memory_->set_byte(0x5002, reader_->data_init_address() >> 8);
    memory_->set_byte(0x5003, 0xF2); // stp

    memory_->set_byte(0x5004, 0x20); // jsr
    memory_->set_byte(0x5005, reader_->data_play_address() & 0xFF);
    memory_->set_byte(0x5006, reader_->data_play_address() >> 8);
    memory_->set_byte(0x5007, 0x4C); // jmp 0x5003
    memory_->set_byte(0x5008, 0x03);
    memory_->set_byte(0x5009, 0x50);


    // call init
    #ifdef DEBUG
    print_header(0);
    #endif
    register_pc_ = 0x5000;
    for(;;) {
        uint8_t opcode = memory_->get_byte(register_pc_++);
        if(opcode == 0xF2) { // stp
            #ifdef DEBUG
            instr_pc_ = register_pc_-1;
            opcode_ = opcode;
            instr_ = "(HLT)";
            implied();
            print_state();
            #endif
            break;
        }
        int cycles = run_instruction(opcode);
        #ifdef DEBUG
        print_state();
        #endif
        for(int i = 0; i < cycles; i++) {
            apu_->CPUClock();
        }
    }
    #ifdef DEBUG
    print_header(num_calls_);
    #endif
}

double NesCpu::play_gap() {
    // call play ever 1,000,000 / period times a second
    double rate = 1000000.0;
    if (reader_->is_pal()) {
        rate /= reader_->pal_speed();
    } else { // ntsc
        //cout << reader_->ntsc_speed() << endl;
        rate /= reader_->ntsc_speed();
    }
    return 1.0 / rate;
}

/**
* @breif nsf (music file) player
*/
bool NesCpu::play_nsf(int &cycles_left) {
    //clock_t start = clock();
    //if(num_calls_ > 1) { return; }
    for(;;) {
        uint8_t opcode = memory_->get_byte(register_pc_++);
        if(opcode == 0xF2) { // stp
            #ifdef DEBUG
            instr_pc_ = register_pc_-1;
            opcode_ = opcode;
            instr_ = "(HLT)";
            implied();
            print_state();
            #endif
            break;
        }
        int cycles = run_instruction(opcode);
        total_cycles_ += cycles;
        #ifdef DEBUG
        print_state();
        #endif
        // for(int i = 0; i < cycles; i++) {
        //     apu_->CPUClock();
        // }
        cycles_left -= cycles;
        if(cycles_left < 0) {
            return false;
        }
    }
    num_calls_++;
    //printf("finished play %d\n", num_calls_);
    #ifdef DEBUG
    print_header(num_calls_);
    #endif
    return true;
    //printf("total cycles = %d\n", total_cycles_);
    // #ifdef DEBUG
    // if (num_calls_ > 1000) {
    //     return;
    // }
    // #endif
}
