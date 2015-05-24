#!/usr/bin/env python

import re

NES_CPU_FILE = '../cpu/nes_cpu.cc'

def main():
    cpu_file_contents = ''
    with open(NES_CPU_FILE, 'r') as f:
        cpu_file_contents = f.read()

    instructions = cpu_file_contents.split('* INSTRUCTIONS *')[1].split('* HELPER FUNCTIONS *')[0]

    #matches = re.findall(r'NesCpu::([a-z]*)', instructions)
    matches = re.findall(r'(NesCpu::([a-z]*).*\{.*)', instructions)
    print matches


if __name__ == '__main__':
    main()
