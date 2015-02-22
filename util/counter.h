#ifndef COUNTER_H
#define COUNTER_H

#include "countable.h"

class Counter {
    int value_ = 0;
    int reload_ = 0;
    bool halt_ = false;
    bool loop_ = true;
    bool down_ = true;
    bool reload_flag_ = false;
    bool clear_reload_ = false;
    Countable *caller_;
    Countable *reload_caller_;
    int reset_timer_ = 0; // not used internally

public:
    Counter(int reload, bool loop, Countable *caller, bool down,
            Countable *reload_caller);
    void Clock();
    int value() {return value_;}
    int reload() {return reload_;}
    bool halt() {return halt_;}
    bool reload_flag() {return reload_flag_;}

    void set_value(int value) {value_ = value;}
    void set_reload(int reload) {reload_ = reload;}
    void set_halt_(bool halt) {halt_ = halt;}
    void set_loop(bool loop) {loop_ = loop;}
    void set_down(bool down) {down_ = down;}
    void enable_reload_flag(bool keep=false) {
        reload_flag_ = true; 
        clear_reload_ = !keep;}
    void clear_reload_flag_next_clock() {clear_reload_ = true;}

    int reset_timer() {return reset_timer_;}
    void set_reset_timer(int t) {reset_timer_ = t;}
};

#endif
