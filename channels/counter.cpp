#include "counter.h"

Counter::Counter(int reload, bool loop=true, 
                 void (*callback)()=NULL, bool down=true) {}

Counter::Clock() {
    if (halt_) {return;}
    if (reload_flag_) {
        value_ = reload_;
    } else {
        if (value >= 0 && value <= reload_)
            value_ -= down_; // abusive
        if (loop_ && (value_ < 0 || value > reload_)) {
            if(down) {value_ = reload_;}
            else {value_ = 0;}
            if (callback_)
                callback_();
        }
    }
    if (clear_reload_) {
        reload_flag_ = false;
        clear_reload_ = false;
    }
}
