#!/usr/bin/env python

import re

NES_CPU_FILE = '../cpu/nes_cpu.cc'


def main():
    cpu_file_contents = ''
    with open(NES_CPU_FILE, 'r') as f:
        cpu_file_contents = f.read()

    instructions = cpu_file_contents.split('* INSTRUCTIONS *')[1].split(
        '* HELPER FUNCTIONS *')[0]

    for line in instructions.split('\n'):
        matches = re.findall(r'(NesCpu::([a-zA-z]*).*\{.*)', line)
        print line
        if matches:
            instr = matches[0][1]
            print '    instr_ = "' + instr.upper() + '";'

if __name__ == '__main__':
    main()
