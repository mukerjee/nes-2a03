class Noise:
    def __init__(self):
        self.LENGTH_COUNTER_TABLE = \
            [10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
             12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30]

        self.PERIOD_TABLE = \
            [4, 8, 16, 32, 64, 96, 128, 160,
             202, 254, 380, 508, 762, 1016, 2034, 4068]
        
        self.length_counter_disable_or_envelope_loop_flag = 1
        self.envelope_disable = 1
        self.volume_or_envelope_period = 0  # range is 0 - F

        self.envelope_restart = 1
        self.envelope_divider_counter = 0
        self.envelope_counter = 0
        
        self.period_bitload = 0  # range is 0 - F
        self.timer_counter = 0
        
        self.mode_flag = 0
        self.shift_register_value = 1

        self.length_counter_bitload = 0  # range is 0 - 1F
        self.length_counter_counter = 0

        self.shift_register_gate_open = 0
        self.length_counter_gate_open = 0

        self.enabled = 1

    def set_400C(self, b):  # len counter disable, env disable, volume
        b = b & 255  # only one byte
        self.length_counter_disable_or_envelope_loop_flag = (b & 32) >> 5
        self.envelope_disable = (b & 16) >> 4
        self.volume_or_envelope_period = b & 15

    def set_400E(self, b):  # mode, period
        b = b & 255  # only one byte
        self.mode_flag = b & 128 >> 7
        self.period_bitload = b & 15
        self.timer_counter = self.PERIOD_TABLE[self.period_bitload]

    def set_400F(self, b):  # length counter bitload
        b = b & 255  # only one byte
        self.length_counter_bitload = (b & 248) >> 3
        if self.enabled:
            self.length_counter_counter = \
                self.LENGTH_COUNTER_TABLE[self.length_counter_bitload]
        self.envelope_restart = 1
        
    def set_enabled(self, enabled):
        self.enabled = enabled
        if not self.enabled:
            self.length_counter_counter = 0
            self.length_counter_gate_open = 0

    def get_length_counter(self):
        return self.length_counter_counter != 0

    def envelope_clock(self):  # called in quarter frames
        if self.envelope_restart:
            self.envelope_restart = 0
            self.envelope_counter = 15
            self.envelope_divider_counter = self.volume_or_envelope_period
        else:
            self.envelope_divider_counter -= 1
            if self.envelope_divider_counter < 0:
                self.envelope_divider_counter = self.volume_or_envelope_period
                if self.envelope_counter:
                    self.envelope_counter -= 1
                elif self.length_counter_disable_or_envelope_loop_flag:
                    self.envelope_counter = 15

    def length_counter_clock(self):  # called in half frames
        if not self.enabled:
            self.length_counter_counter = 0
            self.length_counter_gate_open = 0
        if self.length_counter_disable_or_envelope_loop_flag:
            self.length_counter_gate_open = 0
        elif self.length_counter_counter:
            self.length_counter_counter -= 1
            if self.length_counter_counter == 0:
                self.length_counter_gate_open = 0
            else:
                self.length_counter_gate_open = 1

    def timer_clock(self):  # called every other CPU clock
        self.timer_counter -= 1
        if self.timer_counter < 0:
            self.timer_counter = self.PERIOD_TABLE[self.period_bitload]
            self.shift_register_clock()
    
    def shift_register_clock(self):  # called by timer
        if self.mode_flag:
            extra_bit = (self.shift_register_value & 64) >> 6
        else:
            extra_bit = (self.shift_register_value & 2) >> 1
        feedback = (self.shift_register_value & 1) ^ extra_bit
        self.shift_register_value = self.shift_register_value >> 1
        self.shift_register_value += (feedback << 14)

        if self.shift_register_value & 1 == 1:
            self.shift_register_gate_open = 0
        else:
            self.shift_register_gate_open = 1

    def get_envelope_volume(self):
        if self.envelope_disable:
            return self.volume_or_envelope_period
        else:
            return self.envelope_counter

    def get_current(self):
        if self.shift_register_gate_open and self.length_counter_gate_open:
            return self.get_envelope_volume(self)
        else:
            return 0
