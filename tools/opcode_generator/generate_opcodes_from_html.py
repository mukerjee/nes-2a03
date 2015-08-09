#!/usr/bin/env python
opcodes = [o.split('(')[0].split()[-1] for o in
           open('opcodes').read().split('\n')[:-1]]

rows = open("6502Opcodes.html").read().split("TABLE")[1].split("<TR>")[2:-1]

print """#ifndef NES_CPU_OPCODE_TABLE_H_
#define NES_CPU_OPCODE_TABLE_H_"

#define NUM_OPCODES 256"

namespace nes_cpu {
    struct OpcodeProperties {
        uint8_t (*op)(uint16_t);
        uint16_t (*addressing_mode)();
        uint8_t cycles;
        uint8_t penalty;  // penalty for crossing a page (cycles)
    };

    const OpcodeProperties opcodes[NUM_OPCODES] = {"""

output = ""
for row in rows:
    for line in row.split("</TR>")[0].split("\n")[2:-1]:
        data = line.split("</FONT>")[0].split("<BR>")
        opcode = data[0].split(">")[-1]
        if len(data) == 1:
            output += '        {},\n'
            continue
        params = data[1].split("<")[0].split()
        if len(params) > 1:
            addressing_mode = params[0]
            cycles = params[1]
        else:
            if opcode == 'asl' or opcode == 'lsr' or \
               opcode == 'rol' or opcode == 'ror':
                addressing_mode = 'acc'
            else:
                addressing_mode = 'imp'
            cycles = params[0]
        if cycles[-1] == '*':
            penalty = "1"
            cycles = cycles[:-1]
        else:
            penalty = "0"

        ad = {'imp': 'AMImplied', 'acc': 'AMAccumulator', 'imm': 'AMImmediate',
              'zp': 'AMZeroPage', 'zpx': 'AMZeroPageX',
              'zpy': 'AMZeroPageY', 'rel': 'AMRelative',
              'abs': 'AMAbsoulte', 'abx': 'AMAbsoluteX',
              'aby': 'AMAbsoluteY', 'ind': 'AMIndirect',
              'izx': 'AMIndexedIndirect',
              'izy': 'AMIndirectIndexed'}

        addressing_mode = ad[addressing_mode]

        if opcode in opcodes:
            output += "        {.op=&" + opcode + ", .addressing_mode = &" + \
                      addressing_mode + ", .cycles = " + cycles + \
                      ", .cross_page_penalty = " + penalty + "},\n"
        else:
            output += "        {},\n"

print output[:-2]

print """    }
}
#endif  // NES_CPU_OPCODE_TABLE_H_
"""
