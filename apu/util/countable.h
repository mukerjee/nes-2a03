#ifndef COUNTABLE_H
#define COUNTABLE_H

#include "counter.h"

class Counter;

class Countable {
 public:
    virtual void CounterCallback(Counter *c) = 0;
    virtual void CounterReloadCallback(Counter *c) = 0;
};

#endif
