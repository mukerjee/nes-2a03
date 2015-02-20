#!/usr/bin/env python

import os
import sys
import argparse
import logging
import struct


class NSFReader(object):
    '''Class for reading and interpreting NSF audio files'''
    
    def __init__(self, nsffile):
        logging.info('Reading NSF file: %s' % nsffile)
        with open(nsffile, 'r') as f:

            # first 128 bytes are header (http://kevtris.org/nes/nsfspec.txt):
            #   <   little endian   (make sure NSF is really little endian)
            #   4s  4 char string, always NESM
            #   B   one byte, always 1A
            #   B   version number
            #   B   number of songs
            #   B   starting song
            #   H   load address of data
            #   H   init address of data
            #   H   play address of data
            #   32s name of song
            #   32s artist
            #   32s copyright holder
            #   H   speed, in 1/1000000th sec ticks, NTSC
            #   BBBBBBBB  Bankswitch Init Values
            #   H   speed, in 1/1000000th sec ticks, PAL
            #   B   PAL/NTSC bits:
            #         bit 0: if clear, this is an NTSC tune
            #         bit 0: if set, this is a PAL tune
            #         bit 1: if set, this is a dual PAL/NTSC tune
            #         bits 2-7: not used. they *must* be 0
            #   B   Extra Sound Chip Support
            #         bit 0: if set, this song uses VRCVI
            #         bit 1: if set, this song uses VRCVII
            #         bit 2: if set, this song uses FDS Sound
            #         bit 3: if set, this song uses MMC5 audio
            #         bit 4: if set, this song uses Namco 106
            #         bit 5: if set, this song uses Sunsoft FME-07
            #         bits 6,7: future expansion: they *must* be 0
            #   BBBB    for expansion, must be 0
            header_data = f.read(128)
            header = struct.unpack('<4sBBBBHHH32s32s32sHBBBBBBBBHBBBBBB', header_data)

            # sanity checks
            if header[0] != 'NESM':
                logging.warn('NSF header did not begin with "NESM"')
            if header[1] != 0x1A:
                logging.warn('Unexpected value in 5th byte of NSF header')
            # TODO: finish sanity checks

            self.nsf_version = header[2]
            self.num_songs = header[3]
            self.starting_song = header[4]
            self.data_load_addr = header[5]  # make private?
            self.data_init_addr = header[6]  # make private?
            self.data_play_addr = header[7]  # make private?
            self.name = header[8].split('\x00')[0]
            self.artist = header[9].split('\x00')[0]
            self.copyright_holder = header[10].split('\x00')[0]
            # TODO: finish interpreting header fields




def main():
    '''For testing the NSFReader class'''
    reader = NSFReader(args.nsffile)
    print reader.name


if __name__ == "__main__":
    # set up command line args
    parser = argparse.ArgumentParser(formatter_class=argparse.ArgumentDefaultsHelpFormatter,\
                                     description='Play back NSF audio files.')
    parser.add_argument('nsffile', help='NSF audio file')
    parser.add_argument('-q', '--quiet', action='store_true', default=False, help='only print errors')
    parser.add_argument('-v', '--verbose', action='store_true', default=False, help='print debug info. --quiet wins if both are present')
    args = parser.parse_args()
    

    # set up logging
    if args.quiet:
        level = logging.WARNING
    elif args.verbose:
        level = logging.DEBUG
    else:
        level = logging.INFO
    config = {
        'format' : "%(levelname) -10s %(asctime)s %(module)s:%(lineno) -7s %(message)s",
        'level' : level
    }
    logging.basicConfig(**config)
    
    main()
