#ifndef COUNTER_H
#define COUNTER_H

class Counter {
    int value_ = 0;
    int reload_ = 0;
    bool loop_ = true;
    void (*callback_()) = NULL;
    bool down_ = true;
    bool reload_flag_ = false;
    bool halt_ = false;
    bool clear_reload_ = false;

public:
    Counter(int reload, bool loop, void (*callback)(), bool down);
    void Clock();
    int value() {return value_;}
    bool halt() {return halt_;}
    bool reload_flag() {return reload_flag_;}
    void set_value(int value) {value_ = value;}
    void set_reload(int reload) {reload_ = reload;}
    void set_loop(bool loop) {loop_ = loop;}
    void enable_reload_flag(bool keep) {reload_flag_ = true; clear_reload_ = !keep;}
    void set_halt_(bool halt) {halt_ = halt;}
    void clear_reload_flag_next_clock() {clear_reload_ = true;}
};

#endif
