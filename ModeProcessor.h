#pragma once

class ModeProcessor {
public:
    virtual ~ModeProcessor() {}
    virtual void process() = 0;
};
