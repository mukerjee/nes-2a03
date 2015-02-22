#!/usr/bin/env python

from pulse import Pulse
from triangle import Triangle
from noise import Noise
from dmc import DMC

from apu_mixer import apu_mixer
from resample import resample
from resample import write_wave

import sys
import time


class APU:
    def __init__(self):
        self.pulse1 = Pulse(False)
        self.pulse2 = Pulse(True)
        self.triangle = Triangle()
        self.noise = Noise()
        self.dmc = DMC()

        self.frame_counter_irq_inhibit = 0
        self.frame_counter_interrupt = 0
        self.frame_counter_mode = 0
        self.frame_counter_divider_counter = 0
        self.frame_counter_sequencer_counter = 0
        self.frame_counter_sequencer_counter_reset_timer = 0
        self.frame_sequencer_please_quarter_clock = 0
        self.frame_sequencer_please_half_clock = 0

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

    def set_4000(self, b):
        self.pulse1.set_4000(b)
    def set_4001(self, b):
        self.pulse1.set_4001(b)
    def set_4002(self, b):
        self.pulse1.set_4002(b)
    def set_4003(self, b):
        self.pulse1.set_4003(b)

    def set_4004(self, b):
        self.pulse2.set_4000(b)
    def set_4005(self, b):
        self.pulse2.set_4001(b)
    def set_4006(self, b):
        self.pulse2.set_4002(b)
    def set_4007(self, b):
        self.pulse2.set_4003(b)

    def set_4008(self, b):
        self.triangle.set_4008(b)
    def set_400A(self, b):
        self.triangle.set_400A(b)
    def set_400B(self, b):
        self.triangle.set_400B(b)

    def set_400C(self, b):
        self.noise.set_400C(b)
    def set_400E(self, b):
        self.noise.set_400E(b)
    def set_400F(self, b):
        self.noise.set_400F(b)

    def set_4010(self, b):
        self.dmc.set_4010(b)
    def set_4011(self, b):
        self.dmc.set_4011(b)
    def set_4012(self, b):
        self.dmc.set_4012(b)
    def set_4013(self, b):
        self.dmc.set_4013(b)
        
    def set_4015(self, b):  # enable individual channels
        b = b & 255  # only one byte
        self.pulse1.set_enabled(b & 1)
        self.pulse2.set_enabled((b & 2) >> 1)
        self.triangle.set_enabled((b & 4) >> 2)
        self.noise.set_enabled((b & 8) >> 3)
        self.dmc.set_enabled((b & 16) >> 4)
 
    def set_4017(self, b):  # frame counter
        b = b & 255  # only one byte
        self.frame_counter_mode = (b & 128) >> 7
        self.frame_counter_irq_inhibit = (b & 64) >> 6
        if self.frame_counter_irq_inhibit:
            self.frame_counter_interrupt = 0
        if self.frame_counter_mode:
            self.frame_sequencer_half_clock()
            self.frame_sequencer_quarter_clock()
        if self.frame_counter_divider_counter:  # middle of APU cycle
            self.frame_counter_sequencer_counter_reset_timer = 3
        else:
            self.frame_counter_sequencer_counter_reset_timer = 4

    def get_4015(self):
        p1 = self.pulse1.get_length_counter()
        p2 = self.pulse2.get_length_counter()
        t = self.triangle.get_length_counter()
        n = self.noise.get_length_counter()
        d = self.dmc.get_active()
        i = self.dmc.get_interrupt()
        f = self.frame_counter_interrupt
        # TODO
        # I don't handle the case where an interrupt happens during a read
        # this should result in the interrupt being read but not having it
        # reset
        self.frame_counter_interrupt = 0

        return p1 + (p2 << 1) + (t << 2) + (n << 3) + (d << 4) + \
            (i << 6) + (f << 7)

    def frame_sequencer_quarter_clock(self):
        self.pulse1.envelope_clock()
        self.pulse2.envelope_clock()
        self.noise.envelope_clock()
        self.triangle.linear_counter_clock()

    def frame_sequencer_half_clock(self):
        self.pulse1.length_counter_clock()
        self.pulse2.length_counter_clock()
        self.triangle.length_counter_clock()
        self.noise.length_counter_clock()

        self.pulse1.sweep_clock()
        self.pulse2.sweep_clock()

    def frame_sequencer_clock(self):
        self.frame_counter_sequencer_counter += 1
        if self.frame_counter_sequencer_counter == 3728:
            self.frame_sequencer_please_quarter_clock = 1
        if self.frame_counter_sequencer_counter == 7456:
            self.frame_sequencer_please_quarter_clock = 1
            self.frame_sequencer_please_half_clock = 1
        if self.frame_counter_sequencer_counter == 11185:
            self.frame_sequencer_please_quarter_clock = 1
        if self.frame_counter_sequencer_counter == 14914 \
           and self.frame_counter_mode:
            self.frame_sequencer_please_quarter_clock = 1
            self.frame_sequencer_please_half_clock = 1
            if not self.frame_counter_irq_inhibit:
                self.frame_counter_interrupt = 1
            self.frame_counter_sequencer_counter = -1
        if self.frame_counter_sequencer_counter == 18640:
            self.frame_sequencer_please_quarter_clock = 1
            self.frame_sequencer_please_half_clock = 1

    def cpu_clock(self):
        if self.frame_counter_sequencer_counter_reset_timer:
            self.frame_counter_sequencer_counter_reset_timer -= 1
            if self.frame_counter_sequencer_counter_reset_timer == 0:
                self.frame_counter_sequencer_counter = 0
        if self.frame_sequencer_please_half_clock:
            self.frame_sequencer_half_clock()
            self.frame_sequencer_please_half_clock = 0
        if self.frame_sequencer_please_quarter_clock:
            self.frame_sequencer_quarter_clock()
            self.frame_sequencer_please_quarter_clock = 0
        self.frame_counter_divider_counter -= 1
        if self.frame_counter_divider_counter < 0:
            self.frame_counter_divider_counter = 1
            self.frame_sequencer_clock()
            self.pulse1.timer_clock()
            self.pulse2.timer_clock()
            self.noise.timer_clock()
            self.dmc.timer_clock()
        self.triangle.timer_clock()

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

    def get_current(self):
        return (self.pulse1.get_current(), self.pulse2.get_current(),
                self.triangle.get_current(), self.noise.get_current(),
                self.dmc.get_current())


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
