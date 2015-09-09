#ifndef NES_NES_H_
#define NES_NES_H_

#include <stdint.h>

#include "cpu.h"
#include "memory.h"
#include "apu.h"
#include "audio_adapter.h"

#define CLOCK_SPEED 1789773  // Hz (NTSC)

class Cpu;
class Apu;

class Nes {
    friend class Cpu;
    friend class DMC;
 public:
    Nes(int sample_rate);

    void RunPeriodic(float interval) {periodic_ = int(interval * CLOCK_SPEED);}
    void LoadCart(const uint8_t* cart, const uint16_t start_address,
                  const size_t length) {memory_->LoadCart(cart, start_address, length);}
    void LoadBank(const uint8_t* bank, const uint8_t bank_number,
                  const size_t length) {memory_->LoadBank(bank, bank_number, length);}
    void Run();

    // TODO: Remove this and set them purely from 6502 assembly.
    void SetRegisters(uint8_t a, uint8_t x, uint8_t y, uint8_t s, uint16_t pc);
    void InitSoundRegisters();

    void SetLogging(std::string file_name, int track);
    void SetLogChecking(std::string correct_log);
    
    AudioAdapter *audio_adapter_;

 private:
    void SetByte(const uint16_t addr, const uint8_t byte);
    uint8_t GetByte(const uint16_t addr);
    uint16_t GetWord(const uint16_t addr);

    void CountSamples(float sample) {number_of_samples_++;}
    
    Cpu *cpu_;
    Memory *memory_;
    Apu *apu_;

    int sample_rate_ = 44100;
    int periodic_ = 0;
    int cycles_since_last_period_ = 0;
    uint64_t number_of_samples_ = 0;
};

#endif  // NES_NES_H_
