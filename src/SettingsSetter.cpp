#include "SettingsSetter.h"

#include "DisplayLine.h"
#include "Tools.h"

namespace {
constexpr uint16_t kMaxLagTime = 2000;
} // namespace

SettingsSetter::SettingsSetter() : m_lagTime(gSettings.lagTime), m_demoStartBeep(millis()), m_timer(BEEPER, RELAY) {}

SettingsSetter::~SettingsSetter() {
    analogWrite(BEEPER, 0);
    gSettings.lagTime = m_lagTime;
    gSettings.updateEEPROM();
}

void SettingsSetter::processSetLagTime() {
    gSettings.lagTime = 0;
    m_timer.tick();

    gDisplay[0] << "Lag time";
    uint8_t lagDecSecs = m_lagTime / 100;
    getInt(lagDecSecs, 0, kMaxLagTime / 100);
    m_lagTime = lagDecSecs * 100;

    if (m_timer.state() != Timer::STOPPED) {
        m_timer.printFormatedState();
        return;
    }

    gDisplay[1] << m_lagTime;

    if (gStartBtn.click())
        m_timer.start(m_lagTime);
}

void SettingsSetter::processSetBeepVolume() {
    gDisplay[0] << "Beep volume";
    uint8_t userVolume = min(gSettings.beepVolume, 30) / 3;
    if (getInt(userVolume, 1, 10))
        m_demoStartBeep = millis();
    gSettings.beepVolume = userVolume * 3;
    gDisplay[1] << userVolume;

    if ((millis() - m_demoStartBeep) % 1000 < 100)
        analogWrite(BEEPER, gSettings.beepVolume);
    else
        analogWrite(BEEPER, 0);
}

void SettingsSetter::processSetAutoFinishView() {
    gDisplay[0] << "Auto finish view";
    getInt(gSettings.autoFinishViewMinutes, 0, 10);

    if (gSettings.autoFinishViewMinutes == 0) {
        gDisplay[1] << "No";
        return;
    }

    gDisplay[1] << gSettings.autoFinishViewMinutes << " minute" << (gSettings.autoFinishViewMinutes == 1 ? "" : "s");
}

void SettingsSetter::processSetBacklight() {
    gDisplay[0] << "Backlight";
    uint8_t userBacklight = min(gSettings.backlight, 50) / 5;
    getInt(userBacklight, 1, 10);
    gSettings.backlight = userBacklight * 5;
    gDisplay[1] << userBacklight;
    analogWrite(BACKLIGHT, gSettings.backlight);
}

void SettingsSetter::processStartWithSettings() {
    gDisplay[0] << "Start with stngs";
    uint8_t choice = gSettings.startWithSettings;
    getInt(choice, 0, 1);
    gSettings.startWithSettings = choice;
    if (choice == 0)
        gDisplay[1] << "No";
    else
        gDisplay[1] << "Yes";
}

void SettingsSetter::process() {
    if (gModeSwitchBtn.click() && m_timer.state() != Timer::RUNNING) {
        m_step = (Step)(((int)m_step + 1) % (int)Step::last_);

        analogWrite(BEEPER, 0);
        m_demoStartBeep = millis();
        gSettings.lagTime = m_lagTime;
        gSettings.updateEEPROM();
    }

    switch (m_step) {
    case Step::setLagTime:
        processSetLagTime();
        break;
    case Step::setBacklight:
        processSetBacklight();
        break;
    case Step::setBeepVolume:
        processSetBeepVolume();
        break;
    case Step::setAutoFinishView:
        processSetAutoFinishView();
        break;
    case Step::setStartWithSettings:
        processStartWithSettings();
        break;
    }
}
