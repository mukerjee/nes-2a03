#ifndef RESAMPLE_H
#define RESAMPLE_H

#include <vector>
#include <math.h>
#include <stdio.h>

#define CLOCK_SPEED 1789773  // Hz (NTSC)
using namespace std;

void resample(const vector<float> data, const int byte_depth,
              const int sample_rate, vector<int16_t> &sample_output);

void write_wave(const char *output_file, const int16_t bit_depth,
                const int32_t sample_rate, const vector<int16_t> data);

#endif
