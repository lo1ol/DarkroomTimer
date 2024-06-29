#include "SettingsSetter.h"

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

    printFormatedLine("Lag time", 0);
    uint8_t lagDecSecs = m_lagTime / 100;
    getInt(lagDecSecs, 0, kMaxLagTime / 100);
    m_lagTime = lagDecSecs * 100;

    if (m_timer.state() != Timer::STOPPED) {
        m_timer.printFormatedState();
        return;
    }

    printFormatedTime("", m_lagTime);

    if (gStartBtn.click())
        m_timer.start(m_lagTime);
}

void SettingsSetter::processSetBeepVolume() {
    printFormatedLine("Beep volume", 0);
    uint8_t userVolume = min(gSettings.beepVolume, 30) / 3;
    if (getInt(userVolume, 1, 10))
        m_demoStartBeep = millis();
    gSettings.beepVolume = userVolume * 3;
    char str[3] = "";
    concatInt(str, userVolume);
    printFormatedLine(str, 1);

    if ((millis() - m_demoStartBeep) % 1000 < 100)
        analogWrite(BEEPER, gSettings.beepVolume);
    else
        analogWrite(BEEPER, 0);
}

void SettingsSetter::processSetAutoFinishView() {
    printFormatedLine("Auto finish view", 0);
    getInt(gSettings.autoFinishViewMinutes, 0, 10);

    if (gSettings.autoFinishViewMinutes == 0) {
        printFormatedLine("No", 1);
    } else if (gSettings.autoFinishViewMinutes == 1) {
        printFormatedLine("1 minute", 1);
    } else {
        char str[MAX_SYMS_PER_LINE] = "";
        concatInt(str, gSettings.autoFinishViewMinutes);
        concat(str, " minutes");
        printFormatedLine(str, 1);
    }
}

void SettingsSetter::processSetBacklight() {
    printFormatedLine("Backlight", 0);
    uint8_t userBacklight = min(gSettings.backlight, 50) / 5;
    getInt(userBacklight, 1, 10);
    gSettings.backlight = userBacklight * 5;
    char str[3] = "";
    concatInt(str, userBacklight);
    printFormatedLine(str, 1);
    analogWrite(BACKLIGHT, gSettings.backlight);
}

void SettingsSetter::processStartWithSettings() {
    printFormatedLine("Start with stngs", 0);
    uint8_t choice = gSettings.startWithSettings;
    getInt(choice, 0, 1);
    gSettings.startWithSettings = choice;
    if (choice == 0)
        printFormatedLine("No", 1);
    else
        printFormatedLine("Yes", 1);
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
