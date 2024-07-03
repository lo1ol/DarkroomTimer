#pragma once

#include "Time.h"

class ModeProcessor {
public:
    virtual ~ModeProcessor() {}
    virtual void process() = 0;
    virtual void reset() = 0;
    virtual void printLog() = 0;
    virtual void switchMode() = 0;
    virtual bool supportPrintInLog() const = 0;

protected:
    uint8_t printLogHelper(Time (*timeGetter)(void* ctx, uint8_t id, bool& current, bool& end), void* ctx) const;
};
