#pragma once

#include "Time.h"

class ModeProcessor {
public:
    virtual ~ModeProcessor() {}
    virtual void process() = 0;
    virtual void reset() = 0;
    virtual void switchMode() = 0;

    virtual const char* preview() const = 0;

protected:
    void printTimeHelper(Time (*timeGetter)(const void* ctx, uint8_t id, bool& current, const char*& mark),
                           const void* ctx) const;
};
