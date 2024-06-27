#pragma once

class ModeProcessor {
public:
    virtual ~ModeProcessor() {}
    virtual void process() = 0;
    virtual void reset() = 0;
};
