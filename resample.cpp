import wave
import binascii
from math import floor

CLOCK_SPEED = 1789773  # Hz (NTSC)


def tohex(x, bit_width):
    return hex((x + (1 << bit_width)) % (1 << bit_width))


def resample(data, byte_depth, sample_rate):
    # assumes data is real from 0.0 - 1.0
    # and len(data) is the number of clock cycles
    sample_output = []
    total_time = float(len(data)) / CLOCK_SPEED
    total_samples = int(floor(total_time * sample_rate))
    for i in xrange(total_samples):
        wall_time = float(i) / sample_rate
        sample_value = data[int(floor(wall_time * CLOCK_SPEED))]
        
        # mapping to BYTE_DEPTH space
        h = tohex(int(floor(sample_value * (2**(byte_depth*8) - 1) -
                  2**(byte_depth*8-1))), byte_depth*8)[2:]
        
        # little endian
        sample_output.append(h[2:]+h[:2])
    return sample_output


def write_wave(output_file, byte_depth, sample_rate, data):
    data = ''.join(data)
    out_file = wave.open(output_file, 'w')
    out_file.setparams((1, byte_depth, sample_rate, 0, 'NONE', 'None'))
    out_file.writeframes(binascii.a2b_hex(data))
    out_file.close()
