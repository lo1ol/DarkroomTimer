#include "LinearTestMode.h"

#include "../Hardware.h"

LinearTestMode::LinearTestMode(SubMode subMode) : TestMode(subMode) {
    m_initTime = 8_s;
    m_stepTime = 2_s;
}

bool LinearTestMode::handleStepSet() {
    return gEncoder.getTime(m_stepTime);
}

void LinearTestMode::printStep() const {
    gDisplay[1] << F("Step t:") << m_stepTime;
}

Time LinearTestMode::getStepTime_(uint8_t) const {
    return m_stepTime;
}

Time LinearTestMode::getStepTotalTime_(uint8_t step) const {
    return m_initTime + m_stepTime * static_cast<uint16_t>(step);
}

const __FlashStringHelper* LinearTestMode::header() const {
    switch (kSubMode) {
    case Generic:
        return F("Linear test");
    case SplitGrade:
        return F("Splt linear test");
    case Local:
        return F("Locl linear test");
    case Expert:
        return F("Xprt linear test");
    }

    assert(false);
    return F("");
}
