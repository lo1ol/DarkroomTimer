#include "IdleShower.h"

#include "Hardware.h"

void IdleShower::startAnim(DisplayAnimation::Id id) {
    stopAnim();

    m_anim = DisplayAnimation::createAnimation(id);
    gDisplay.reset();
    gDisplay.tick();
    m_nextUpdateTime = 0;
}

void IdleShower::stopAnim() {
    if (m_anim)
        m_anim->~DisplayAnimation();

    m_anim = nullptr;
}

bool IdleShower::tick() {
    if (!m_anim)
        return true;

    if (gEncoder.getDir()) {
        gEncoder.clear();
        return true;
    }

    if (gEncoderBtn.pressing()) {
        gEncoderBtn.skipEvents();
        return true;
    }

    if (gStartBtn.pressing()) {
        gStartBtn.skipEvents();
        return true;
    }

    if (gViewBtn.pressing()) {
        gViewBtn.skipEvents();
        return true;
    }

    if (gModeBtn.pressing()) {
        gModeBtn.skipEvents();
        return true;
    }

    if (gMillis() >= m_nextUpdateTime)
        m_nextUpdateTime = gMillis() + m_anim->tick();

    return false;
}
