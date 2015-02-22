        self.accum = 0

        self.write_registers = {'4000': self.set_4000, '4001': self.set_4001,
                                '4002': self.set_4002, '4003': self.set_4003,
                                '4004': self.set_4004, '4005': self.set_4005,
                                '4006': self.set_4006, '4007': self.set_4007,
                                '4008': self.set_4008, '400A': self.set_400A,
                                '400B': self.set_400B, '400C': self.set_400C,
                                '400E': self.set_400E, '400F': self.set_400F,
                                '4010': self.set_4010, '4011': self.set_4011,
                                '4012': self.set_4012, '4013': self.set_4013,
                                '4015': self.set_4015, '4017': self.set_4017}

        self.read_registers = {'4015': self.get_4015}

        self.opcodes = {'nop': self.compute_nop, 'lda': self.compute_lda,
                        'sta': self.compute_sta}


    def compute_nop(self, i):
        return

    def compute_lda(self, i):
        i = i.split()
        if i[1][:2] == '#%':
            self.accum = int(i[1][2:], 2)
        elif i[1][:2] == '#$':
            self.accum = int(i[1][2:], 16)
        else:
            print 'error: bad lda'
            sys.exit(-1)

    def compute_sta(self, i):
        i = i.split()
        mem = 0
        if i[1][0] == '$':
            mem = i[1][1:]
        else:
            print 'error: bad sta'
            sys.exit(-1)
        self.write_registers[mem](self.accum)


    def compute(self, i):
        # TODO do some magic here to pass loads and store to the right things
        self.opcodes[i[:3]](i)
        self.cpu_clock()
        return self.get_current()


if __name__ == '__main__':
    apu = APU()

    instr = ['lda #%00000001',
             'sta $4015',
             'lda #%10111111',
             'sta $4000',
             'lda #$C9',
             'sta $4002',
             'lda #$00',
             'sta $4003']

    instr += ['nop']*1789773*5

    start = time.time()
    print 'start'
    output = map(apu.compute, instr)

    print 'done instructions', time.time() - start
    output = apu_mixer(output)
    print 'done mixer', time.time() - start
    output = resample(output, 2, 48000)
    print 'done resample', time.time() - start
    write_wave('test.wav', 2, 48000, output)
    print 'done wave', time.time() - start
