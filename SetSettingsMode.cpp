#include "SetSettingsMode.h"

#include "Tools.h"

namespace {
constexpr uint16_t kMaxLagTime = 2000;
} // namespace

SetSettingsMode::SetSettingsMode() {
    m_step = Step::setLagTime;
    m_lagTime = gSettings.lagTime;
}

SetSettingsMode::~SetSettingsMode() {
    gTimer.resetTotal();
    analogWrite(BEEPER, 0);
    gSettings.lagTime = m_lagTime;
    gSettings.updateEEPROM();
}

void SetSettingsMode::processSetLagTime() {
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
        if (gTimer.state() == Timer::STOPPED) {
            gSettings.lagTime = 0;
            gTimer.start(m_lagTime);
        }
    }
}

void SetSettingsMode::processSetBeepVolume() {
    printFormatedLine("Beep volume", 0);
    getInt(gSettings.beepVolume, 0, 20);
    char str[3] = "";
    concatInt(str, gSettings.beepVolume);
    printFormatedLine(str, 1);
    analogWrite(BEEPER, gSettings.beepVolume);
}

void SetSettingsMode::process() {
    if (gModeSwitchBtn.click() && gTimer.state() != Timer::RUNNING) {
        m_step = (Step)(((int)m_step + 1) % (int)Step::last_);

        gTimer.resetTotal();
        analogWrite(BEEPER, 0);
        gSettings.lagTime = m_lagTime;
        gSettings.updateEEPROM();
    }

    switch (m_step) {
    case Step::setLagTime:
        processSetLagTime();
        break;
    case Step::setBeepVolume:
        processSetBeepVolume();
        break;
    }
}
