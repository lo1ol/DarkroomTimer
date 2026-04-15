#include "IdleShower.h"

#include "Hardware.h"

void IdleShower::startAnimation(DisplayAnimation::Id id) {
    if (m_anim)
        m_anim->~DisplayAnimation();

    m_anim = DisplayAnimation::createAnimation(id);
    gDisplay.reset();
    gDisplay.tick();
    m_nextUpdateTime = 0;
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

    if (millis() >= m_nextUpdateTime)
        m_nextUpdateTime = millis() + m_anim->tick();

    return false;
}
