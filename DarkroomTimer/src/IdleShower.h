#pragma once

#include "DisplayAnimation.h"

class IdleShower {
public:
    void startAnim(DisplayAnimation::Id animId);
    void stopAnim();

    // return true if need to exit
    bool tick();

    bool showingAnim() const { return m_anim; }

private:
    DisplayAnimation* m_anim = nullptr;
    uint32_t m_nextUpdateTime = 0;
};
