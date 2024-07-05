#pragma once

#include "Time.h"

class ModeProcessor {
public:
    virtual ~ModeProcessor() {}
    virtual void process() = 0;
    virtual void reset() = 0;
    virtual void switchMode() = 0;

    virtual bool canSwitchView() const = 0;
    virtual void switchView() = 0;

    virtual void printLog(bool& logOverFlowed) const = 0;

protected:
    uint8_t printLogHelper(Time (*timeGetter)(const void* ctx, uint8_t id, bool& current, bool& end),
                           const void* ctx) const;
};
