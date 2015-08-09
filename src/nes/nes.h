#ifndef NES_NES_H_
#define NES_NES_H_

#define CLOCK_SPEED 1789773  // Hz (NTSC)

class Nes {
 public:
    Nes(int sample_rate) : sample_rate_(sample_rate) {};
    AudioAdapter audio_adapter_;

    void LoadCart(char* cart);
    void RunPeriodic(float interval);
    void Run();

 private:
    Cpu cpu_;
    Memory memory_;
    Apu apu_;

    int sample_rate_;
};

#endif  // NES_NES_H_
