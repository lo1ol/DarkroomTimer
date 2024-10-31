#pragma once

#include "Time.h"

class ModeProcessor {
public:
    virtual ~ModeProcessor() {}
    virtual void process() = 0;
    virtual void reset() = 0;
    virtual void switchMode() = 0;

    virtual const char* preview() const = 0;

    virtual void repaint() const = 0;
};
