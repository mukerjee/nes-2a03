#ifndef DMC_H
#define DMC_H

using namespace std;

class DMC {
 public:
    DMC();
    
    void SetEnabled(bool enabled);

    void Set4010(uint8_t b);
    void Set4011(uint8_t b);
    void Set4012(uint8_t b);
    void Set4013(uint8_t b);
    
    void TimerClock();
    int GetCurrent();
};

#endif
