#ifndef COUNTABLE_H
#define COUNTABLE_H

#include "counter.h"

class Countable {
 public:
    virtual void CounterCallback(Counter *c);
    virtual void CounterReloadCallback(Counter *c);
};

#endif
