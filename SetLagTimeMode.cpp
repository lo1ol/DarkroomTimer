#include "SetLagTimeMode.h"

#include "Tools.h"

#include <EEPROM.h>

namespace {
constexpr uint16_t kMaxLagTime = 2000;
} // namespace

SetLagTimeMode::SetLagTimeMode() {
    m_lagTime = getLagTime();
    gTimer.setLagTime(0);
}

uint16_t SetLagTimeMode::getLagTime() {
    uint16_t lagTime;
    EEPROM.get(0, lagTime);

    if (lagTime > kMaxLagTime)
        return 0;

    return lagTime;
}

SetLagTimeMode::~SetLagTimeMode() {
    EEPROM.put(0, m_lagTime);
    gTimer.setLagTime(m_lagTime);
    gTimer.resetTotal();
}

void SetLagTimeMode::process() {
    printFormatedLine("Lag time", 0);
    uint8_t lagDecSecs = m_lagTime / 100;
    getInt(lagDecSecs, 0, kMaxLagTime / 100);
    m_lagTime = lagDecSecs * 100;
    printFormatedTime("", m_lagTime);

    if (gTimer.state() == Timer::RUNNING) {
        printFormatedTime("", gTimer.left());
    } else {
        printFormatedTime("", m_lagTime);
    }

    if (gStartBtn.click()) {
        if (gTimer.state() == Timer::STOPPED)
            gTimer.start(m_lagTime);
    }
}
