class Triangle:
    def __init__(self):
        # F E D C B A 9 8 7 6 5 4 3 2 1 0 0 1 2 3 4 5 6 7 8 9 A B C D E F
        self.SEQUENCE = list(reversed(range(0, 16))) + range(0, 16)

        self.LENGTH_COUNTER_TABLE = \
            [10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
             12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30]
        
        self.length_counter_disable_or_linear_counter_control = 1
        
        self.linear_counter_bitload = 0  # range is 0 - 127
        self.linear_counter_reload = 1
        self.linear_counter_counter = 0
        
        self.sequencer_counter = 0
        
        self.period_low = 0  # range is 0 - 255
        self.period_high = 0  # range is 0 - 7
        self.timer_counter = 0

        self.length_counter_bitload = 0  # range is 0 - 1F
        self.length_counter_counter = 0

        self.linear_counter_gate_open = 0
        self.length_counter_gate_open = 0

        self.enabled = 1

    def set_4008(self, b):  # length counter disable, linear counter load
        b = b & 255  # only one byte
        self.length_counter_disable_or_linear_counter_control = (b & 128) >> 7
        self.linear_counter_reload = b & 127

    def set_400A(self, b):  # period low
        self.period_low = b & 255
        self.sweep_get_target()

    def set_400B(self, b):  # length counter bitload, period high
        b = b & 255  # only one byte
        self.length_counter_bitload = (b & 248) >> 3
        if self.enabled:
            self.length_counter_counter = \
                self.LENGTH_COUNTER_TABLE[self.length_counter_bitload]
        self.period_high = b & 7
        self.linear_counter_reload = 1
        
    def set_enabled(self, enabled):
        self.enabled = enabled
        if not self.enabled:
            self.length_counter_counter = 0
            self.length_counter_gate_open = 0

    def get_length_counter(self):
        return self.length_counter_counter != 0

    def linear_counter_clock(self):  # called in quarter frames
        if self.linear_counter_reload:
            self.linear_counter_counter = self.linear_counter_reload
        else:
            if self.linear_counter_counter:
                self.linear_counter_counter -= 1
                if self.linear_counter_counter == 0:
                    self.linear_counter_gate_open = 0
                else:
                    self.linear_counter_gate_open = 1
        if not self.length_counter_disable_or_linear_counter_control:
            self.linear_counter_reload = 0

    def length_counter_clock(self):  # called in half frames
        if not self.enabled:
            self.length_counter_counter = 0
            self.length_counter_gate_open = 0
        if self.length_counter_disable_or_linear_counter_control:
            self.length_counter_gate_open = 0
        elif self.length_counter_counter:
            self.length_counter_counter -= 1
            if self.length_counter_counter == 0:
                self.length_counter_gate_open = 0
            else:
                self.length_counter_gate_open = 1

    def timer_clock(self):  # called every CPU clock
        self.timer_counter -= 1
        if self.timer_counter < 0:
            self.timer_counter = self.period_high << 8 + self.period_low
            self.sequencer_clock()
    
    def sequencer_clock(self):  # called by timer
        if self.linear_counter_gate_open and self.length_counter_gate_open:
            self.sequencer_counter += 1
            if self.sequencer_counter > 31:
                self.sequencer_counter = 0

    def get_current(self):
        return self.SEQUENCE[self.sequencer_counter]
