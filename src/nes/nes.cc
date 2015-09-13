#include "nes.h"

Nes::Nes(int sample_rate) :
    sample_rate_(sample_rate) {
    cpu_ = new Cpu(this, CLOCK_SPEED);
    memory_ = new Memory();
    apu_ = new Apu(this);
    audio_adapter_ = new AudioAdapter(apu_, CLOCK_SPEED, sample_rate);
    
    cpu_->RanCycles.Connect(apu_, &Apu::ClockCycles);
    apu_->RanCycles.Connect(audio_adapter_, &AudioAdapter::RanCycles);
    audio_adapter_->ReceivedSample.Connect(this, &Nes::CountSamples);
};

Nes::~Nes() {
    delete cpu_;
    delete memory_;
    delete apu_;
    delete audio_adapter_;
}

void Nes::Stop() {
    destroy_ = true;
}

// void Nes::AccumulateSamples(float sample) {
//     samples_.push_back(sample);
//     if (samples_.size() / float(sample_rate_) > 10.0 && !written_wave_) {
//     printf("here\n");
//         WriteWave("test.wav", samples_);
//         written_wave_ = true;
//     }
// }

// void Nes::WriteWave(const char *output_file, const std::vector<float> data) {
//     FILE *fp = fopen(output_file, "wb");

//     int bit_depth = 16;
//     int header_length = 44;

//     int32_t length = header_length + (data.size() * (bit_depth / 8)) - 8;
//     int32_t length_format_data = 16;
//     int16_t wave_type = 1;
//     int16_t num_channels = 1;
//     int32_t bytes_per_second = (sample_rate_ * bit_depth * num_channels) / 8;
//     int16_t frame_size = (bit_depth * num_channels) / 8;
//     int32_t data_length = length + 8 - header_length;


//     // write the header
//     fwrite("RIFF", sizeof(char), 4, fp);
//     fwrite(&length, sizeof(int32_t), 1, fp);
//     fwrite("WAVE", sizeof(char), 4, fp);
//     fwrite("fmt ", sizeof(char), 4, fp);
//     fwrite(&length_format_data, sizeof(int32_t), 1, fp);
//     fwrite(&wave_type, sizeof(int16_t), 1, fp);
//     fwrite(&num_channels, sizeof(int16_t), 1, fp);
//     fwrite(&sample_rate_, sizeof(int32_t), 1, fp);
//     fwrite(&bytes_per_second, sizeof(int32_t), 1, fp);
//     fwrite(&frame_size, sizeof(int16_t), 1, fp);
//     fwrite(&bit_depth, sizeof(int16_t), 1, fp);
//     fwrite("data", sizeof(char), 4, fp);
//     fwrite(&data_length, sizeof(int32_t), 1, fp);

//     // write the data
//     std::vector<uint16_t> resampled_data;
//     for (int i = 0; i < data.size(); i++) {
//         // mapping to BIT_DEPTH space
//         uint16_t sample_int =
//             uint16_t(floor(((data[i] + 1) / 2.0)  * (pow(2, bit_depth) - 1)
//                           - pow(2, bit_depth-1)));
//         resampled_data.push_back(sample_int);
//     }
//     int bytes_written = fwrite(&resampled_data[0], bit_depth / 8,
//                                resampled_data.size(), fp);
//     if (bytes_written != data.size())
//         printf("error!\n");

//     fclose(fp);
// }

void Nes::Run() {
    int new_cycles;
    for(;;) {
        while(number_of_samples_ < sample_rate_ * 5) {
            if (cycles_since_last_period_ >= periodic_) {
                cycles_since_last_period_ -= periodic_;
                new_cycles = cpu_->Run();
                if (new_cycles == -1)
                    return;
                cycles_since_last_period_ += new_cycles;
            } else {
                apu_->ClockCycles(periodic_ - cycles_since_last_period_);
                cycles_since_last_period_ = periodic_;
            }
        }
        // pause periodically...
        int sleep_time = 0;
        int sleep_delay = 2.5*100;
        while (sleep_time < 2.5 * 1000000) {
            usleep(sleep_delay);
            sleep_time += sleep_delay;
            if (destroy_) {
                return;
            }
        }
        number_of_samples_ -= sample_rate_ * 5;
    }
}

void Nes::SetRegisters(uint8_t a, uint8_t x, uint8_t y, uint8_t s, uint16_t pc) {
    cpu_->register_a_ = a;
    cpu_->register_x_ = x;
    cpu_->register_y_ = y;
    cpu_->register_s_ = s;
    cpu_->register_pc_ = pc;
}

void Nes::InitSoundRegisters() {
    for (int i = 0x4000; i < 0x4014; i++) {
        SetByte(i, 0);
    }
    SetByte(0x4015, 0x0F);
}

void Nes::SetLogging(std::string file_name, int track) {
    cpu_->SetLogging(file_name, track);
}

void Nes::SetLogChecking(std::string correct_log) {
    cpu_->SetLogChecking(correct_log);
}

void Nes::SetByte(const uint16_t addr, const uint8_t byte) {
    if (addr >= 0x5FF8 and addr <= 0x5FFF) { // bank switching
        memory_->BankSwitch(addr - 0x5FF8, byte); 
    } else
    if (addr >= 0x4000 and addr <= 0x4017) { // APU
        apu_->SetByte(addr, byte);
    } else {
        memory_->SetByte(addr, byte);
    }
}

uint8_t Nes::GetByte(const uint16_t addr) {
    if (addr == 0x4015) {  // APU status
        return apu_->Get4015();
    }
    return memory_->GetByte(addr);
}

uint16_t Nes::GetWord(const uint16_t addr) {
    return memory_->GetWord(addr);
}
