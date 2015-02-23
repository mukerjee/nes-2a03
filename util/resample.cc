#include "resample.h"

void resample(const vector<float> data, const int byte_depth, 
              const int sample_rate, vector<int16_t> &sample_output) {
    // assumes data is real from 0.0 - 1.0
    // and len(data) is the number of clock cycles
    float total_time = float(data.size()) / CLOCK_SPEED;
    int total_samples = int(floor(total_time * sample_rate));
    for (int i = 0; i < total_samples; i++) {    
        float wall_time = float(i) / sample_rate;
        int sample_value = data[int(floor(wall_time * CLOCK_SPEED))];
        
        // mapping to BYTE_DEPTH space
        int16_t sample_int = 
            int16_t(floor(sample_value * (pow(2, byte_depth*8) - 1) 
                          - pow(2, byte_depth*8-1)));
        sample_output.push_back(sample_int);
    }
}

// TODO: assumes data is of bit_depth 16
void write_wave(const char *output_file, const int16_t bit_depth,
                const int32_t sample_rate, const vector<int16_t> data) {
    FILE *fp = fopen(output_file, "wb");
    
    int header_length = 44;

    int32_t length = 0;
    int32_t length_format_data = 16;
    int16_t wave_type = 1;
    int16_t num_channels = 1;
    int32_t bytes_per_second = (sample_rate * bit_depth * num_channels) / 8;
    int16_t frame_size = (bit_depth * num_channels) / 8;
    int32_t data_length = 0;


    // write the header
    fwrite("RIFF", sizeof(char), 4, fp);
    fwrite(&length, sizeof(int32_t), 1, fp);
    fwrite("WAVE", sizeof(char), 4, fp);
    fwrite("fmt ", sizeof(char), 4, fp);
    fwrite(&length_format_data, sizeof(int32_t), 1, fp);
    fwrite(&wave_type, sizeof(int16_t), 1, fp);
    fwrite(&num_channels, sizeof(int16_t), 1, fp);
    fwrite(&sample_rate, sizeof(int32_t), 1, fp);
    fwrite(&bytes_per_second, sizeof(int32_t), 1, fp);
    fwrite(&frame_size, sizeof(int16_t), 1, fp);
    fwrite(&bit_depth, sizeof(int16_t), 1, fp);
    fwrite("data", sizeof(char), 4, fp);
    fwrite(&data_length, sizeof(int32_t), 1, fp);

    // write the data
    for(vector<int16_t>::const_iterator it = data.begin(); 
        it != data.end(); ++it) {
        fwrite(&*it, bit_depth / 8, 1, fp);
    }

    length = ftell(fp) - 8;
    data_length = ftell(fp) - 44;
    fseek(fp, 4, 0);
    fwrite(&length, sizeof(int32_t), 1, fp);
    fseek(fp, 40, 0);
    fwrite(&data_length, sizeof(int32_t), 1, fp);
    
    fclose(fp);
}

// int main() {
//     vector<int16_t> data;
//     for (int i = 0; i < 48000*5; i++) {
//         data.push_back(0);
//     }
//     write_wave("test.wav", 16, 48000, data);
//     return 0;
// }
