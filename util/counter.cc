#include "counter.h"

void Counter::Clock() {
    if (halt_) {return;}
    if (reload_flag_) {
        if (!value_ and reload_caller_)
            reload_caller_->CounterReloadCallback(this);
        value_ = reload_;
    } else {
        if (value_ >= 0 && value_ <= reload_)
            value_ -= down_; // abusive
        if (loop_ && (value_ < 0 || value_ > reload_)) {
            if(down_) {value_ = reload_;}
            else {value_ = 0;}
            if (caller_)
                caller_->CounterCallback(this);
        }
    }
    if (clear_reload_) {
        reload_flag_ = false;
        clear_reload_ = false;
    }
}
