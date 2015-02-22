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

    header = struct.unpack('BBBHHH32s32s32sHBBBBBBBBHBBBBBB', header_data);

    // sanity checks
    if(strncmp(header_magic_string, MAGIC_STRING, 4))        
        printf("NSF header did not begin with \"NESM\"");
    if(strncmp(&header_magic_byte, MAGIC_BYTE, 1))
        printf("Unexpected value in 5th byte of NSF header");
    // TODO: finish sanity checks

    fread(&nsf_version_, sizeof(uint8_t), 1, fp);
    fread(&num_songs_, sizeof(uint8_t), 1, fp);
    fread(&starting_song_, sizeof(uint8_t), 1, fp);
    fread(&data_load_addr_, sizeof(uint16_t), 1, fp);
    fread(&data_init_addr_, sizeof(uint16_t), 1, fp);
    fread(&data_play_addr_, sizeof(uint16_t), 1, fp);
    fread(&name_, sizeof(char), 32, fp);
    fread(&artist_, sizeof(char), 32, fp);
    fread(&copyright_holder_, sizeof(char), 32, fp);
    // TODO: finish interpreting header fields
}

// int main(int argc, char **argv) {
//     reader = NSFReader(argv[1]);
//     print reader.name;
// }
