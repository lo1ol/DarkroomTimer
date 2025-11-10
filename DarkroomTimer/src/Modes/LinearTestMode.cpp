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
    gDisplay[1] << "Step t:" << m_stepTime;
}

Time LinearTestMode::getStepTime_(uint8_t) const {
    return m_stepTime;
}

Time LinearTestMode::getStepTotalTime_(uint8_t step) const {
    return m_initTime + m_stepTime * static_cast<uint16_t>(step);
}

const char* LinearTestMode::header() const {
    if (kSubMode == SplitGrade)
        return "Splt linear test";
    if (kSubMode == Local)
        return "Locl linear test";
    return "Linear test";
}
