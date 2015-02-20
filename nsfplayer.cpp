#!/usr/bin/env python

import argparse
import logging
import APU
import NSFReader
from apu_mixer import apu_mixer
from resample import resample
from resample import write_wave

BYTE_DEPTH = 2
SAMPLE_RATE = 48000
OUTPUT_FILE = 'test.wav'


def main():
    logging.info('NSF Player launched  [%s]' % args.nsffile)

    apu = APU()
    nsfreader = NSFReader(args.nsffile)
    instr = nsfreader.read_instructions()
    
    # TODO assume instr is a list of instructions
    data = []
    for i in instr:
        data.append(apu.compute(i))

    # TODO should we split up data into chunks before mixing/filtering?
    output = apu_mixer(data)
    sampled_output = resample(output, BYTE_DEPTH, SAMPLE_RATE)
    write_wave(OUTPUT_FILE, BYTE_DEPTH, SAMPLE_RATE, sampled_output)


if __name__ == "__main__":
    # set up command line args
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter,
        description='Play back NSF audio files.')
    parser.add_argument('nsffile', help='NSF audio file')
    parser.add_argument('-i', '--info', help='Print info instead of playing')
    parser.add_argument('-t', '--track', help='Track to play')
    parser.add_argument('-q', '--quiet', action='store_true',
                        default=False, help='only print errors')
    parser.add_argument('-v', '--verbose', action='store_true', default=False,
                        help=('print debug info. '
                              '--quiet wins if both are present'))
    args = parser.parse_args()
    
    # set up logging
    if args.quiet:
        level = logging.WARNING
    elif args.verbose:
        level = logging.DEBUG
    else:
        level = logging.INFO
    config = {
        'format': ("%(levelname) -10s %(asctime)s %(module)s:%(lineno) "
                   "-7s %(message)s"),
        'level': level
    }
    logging.basicConfig(**config)
    
    main()
