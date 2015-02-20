def highpass(data, frequency):  # first order
    # TODO
    return data


def lowpass(data, frequency):  # first order
    # TODO
    return data


pulse_lookup = dict([(n, 95.52 / (8128.0 / n + 100)) for n in xrange(1, 32)] +
                    [(0, 0)])

# This is approximate (within 4% of DMC)
tnd_lookup = dict([(n, 163.67 / (24329.0 / n + 100)) for n in xrange(1, 188)] +
                  [(0, 0)])


def mix((p1, p2, t, n, d)):
    pul = pulse_lookup[p1 + p2]
    tnd = tnd_lookup[3*t + 2*n + d]
    return pul + tnd
    

# operates on lists of fixed size
def apu_mixer(data):
    print len(data)
    output = map(mix, data)
    output = highpass(output, 90)
    output = highpass(output, 440)
    output = lowpass(output, 14000)
    return output
