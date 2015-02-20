class Pulse:
    def __init__(self, pulse2):
        self.SEQ0 = [0, 1, 0, 0, 0, 0, 0, 0]  # 12.5%
        self.SEQ1 = [0, 1, 1, 0, 0, 0, 0, 0]  # 25%
        self.SEQ2 = [0, 1, 1, 1, 1, 0, 0, 0]  # 50%
        self.SEQ3 = [1, 0, 0, 1, 1, 1, 1, 1]  # 25% negated
        self.SEQUENCES = [self.SEQ0, self.SEQ1, self.SEQ2, self.SEQ3]

        self.LENGTH_COUNTER_TABLE = \
            [10, 254, 20, 2, 40, 4, 80, 6, 160, 8, 60, 10, 14, 12, 26, 14,
             12, 16, 24, 18, 48, 20, 96, 22, 192, 24, 72, 26, 16, 28, 32, 30]
        
        self.duty_cycle = 0  # range is 0 - 3
        self.length_counter_disable_or_envelope_loop_flag = 1
        self.envelope_disable = 1
        self.volume_or_envelope_period = 0  # range is 0 - F

        self.envelope_restart = 1
        self.envelope_divider_counter = 0
        self.envelope_counter = 0
        
        self.sequencer_counter = 0

        self.sweep_enabled = 0
        self.sweep_divider_counter = 0
        self.sweep_reload_flag = 0
        self.sweep_divider_period = 1  # range is 1 - 8
        self.sweep_negate_flag = 0
        self.sweep_shift_count = 0  # range is 0 - 7
        
        self.period_low = 0  # range is 0 - 255
        self.period_high = 0  # range is 0 - 7
        self.timer_counter = 0

        self.length_counter_bitload = 0  # range is 0 - 1F
        self.length_counter_counter = 0

        self.enabled = 1
        self.pulse2 = pulse2  # to account for a small difference in sweep unit

    def length_counter_gate_open(self):
        return self.length_counter_counter
    
    def sweep_gate_open(self):
        open = True
        if self.sweep_enabled:
            target_period, raw_period = self.sweep_get_target()
            if raw_period < 8 or target_period > 2047:
                open = False
        return open

    def sequencer_gate_open(self):
        return 0 if (self.period_high << 8) + self.period_low < 8 else \
            self.SEQUENCES[self.duty_cycle][self.sequencer_counter]

    def set_4000(self, b):  # duty, len counter disable, env disable, volume
        b = b & 255  # only one byte
        self.duty_cycle = (b & 192) >> 6
        self.length_counter_disable_or_envelope_loop_flag = (b & 32) >> 5
        self.envelope_disable = (b & 16) >> 4
        self.volume_or_envelope_period = b & 15

    def set_4001(self, b):  # sweep: enabled, div period, neg flag, shift count
        b = b & 255  # only one byte
        self.sweep_enabled = b >> 7
        self.sweep_divider_period = ((b & 112) >> 4)
        self.sweep_negate_flag = (b & 8) >> 3
        self.sweep_shift_count = b & 7
        self.sweep_reload_flag = 1
    
    def set_4002(self, b):  # period low
        self.period_low = b & 255

    def set_4003(self, b):  # length counter bitload, period high
        b = b & 255  # only one byte
        self.length_counter_bitload = (b & 248) >> 3
        if self.enabled:
            self.length_counter_counter = \
                self.LENGTH_COUNTER_TABLE[self.length_counter_bitload]
        self.period_high = b & 7
        self.envelope_restart = 1
        self.sequencer_counter = 0
        
    def set_enabled(self, enabled):
        self.enabled = enabled
        self.length_counter_counter = 0 if not self.enabled else 1

    def get_length_counter(self):
        return self.length_counter_counter != 0

    def sweep_get_target(self):
        raw_period = (self.period_high << 8) + self.period_low
        shift_result = raw_period >> self.sweep_shift_count
        if self.sweep_negate_flag:
            shift_result *= -1
            if not self.pulse2:
                shift_result -= 1
        target_period = raw_period + shift_result
        return target_period, raw_period

    def sweep_clock(self):  # called in half frames
        if self.sweep_reload_flag:
            if self.sweep_enabled and not self.sweep_divider_counter:
                target_period = self.sweep_get_target(self)[0]
                if self.sweep_enabled and self.sweep_shift_count:
                    self.period_high = (target_period & 1792) >> 8
                    self.period_low = target_period & 255

            self.sweep_divider_counter = self.sweep_divider_period
            self.sweep_reload_flag = 0
        else:
            if self.sweep_divider_counter:
                self.sweep_divider_counter -= 1
            else:
                if self.sweep_enabled:
                    self.sweep_divider_counter = self.sweep_divider_period
                    target_period = self.sweep_get_target(self)[0]
                    if self.sweep_enabled and self.sweep_shift_count:
                        self.period_high = (target_period & 1792) >> 8
                        self.period_low = target_period & 255

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
        if not self.length_counter_disable_or_envelope_loop_flag:
            if self.length_counter_counter:
                self.length_counter_counter -= 1

    def timer_clock(self):  # called every other CPU clock
        self.timer_counter -= 1
        if self.timer_counter < 0:
            self.timer_counter = (self.period_high << 8) + self.period_low
            self.sequencer_clock()
    
    def sequencer_clock(self):  # called by timer
        self.sequencer_counter -= 1
        if self.sequencer_counter < 0:
            self.sequencer_counter = 7

    def get_envelope_volume(self):
        if self.envelope_disable:
            return self.volume_or_envelope_period
        else:
            return self.envelope_counter

    def get_current(self):
        if self.sequencer_gate_open() and self.sweep_gate_open() and \
           self.length_counter_gate_open():
            return self.get_envelope_volume()
        else:
            return 0
