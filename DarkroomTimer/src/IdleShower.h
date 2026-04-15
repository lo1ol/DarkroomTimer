#pragma once

#include "DisplayAnimation.h"

class IdleShower {
public:
    void startAnimation(DisplayAnimation::Id animId);

    // return true if need to exit
    bool tick();

private:
    DisplayAnimation* m_anim = nullptr;
    uint32_t m_nextUpdateTime = 0;
};
