#include "SettingsSetter.h"

#include "DisplayLine.h"
#include "Tools.h"

namespace {
constexpr Time kMaxLagTime = 20_ts;
} // namespace

SettingsSetter::SettingsSetter() : m_lagTime(gSettings.lagTime) {}

SettingsSetter::~SettingsSetter() {
    gBeeper.stop();
    gSettings.lagTime = m_lagTime;
    gSettings.updateEEPROM();
}

void SettingsSetter::processSetLagTime() {
    gSettings.lagTime = 0_ts;
    m_timer.tick();

    gDisplay[0] << "Lag time";
    getTime(m_lagTime, true);
    if (m_lagTime > kMaxLagTime)
        m_lagTime = kMaxLagTime;

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
        gBeeper.start();
    gSettings.beepVolume = userVolume * 3;
    gDisplay[1] << userVolume;
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
    uint8_t userBacklight = min(gSettings.backlight, MAX_BACKLIGHT) / (MAX_BACKLIGHT / 10);
    getInt(userBacklight, 1, 10);
    gSettings.backlight = userBacklight * (MAX_BACKLIGHT / 10);
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

void SettingsSetter::processSetViewInTests() {
    gDisplay[0] << "Dflt test view";
    uint8_t choice = gSettings.logViewInTests;
    getInt(choice, 0, 1);
    gSettings.logViewInTests = choice;
    if (choice == 0)
        gDisplay[1] << "Common";
    else
        gDisplay[1] << "Log";
}

void SettingsSetter::processSetViewInMasks() {
    gDisplay[0] << "Dflt mask view";
    uint8_t choice = gSettings.logViewInMasks;
    getInt(choice, 0, 1);
    gSettings.logViewInMasks = choice;
    if (choice == 0)
        gDisplay[1] << "Common";
    else
        gDisplay[1] << "Log";
}

void SettingsSetter::process() {
    if (m_timer.state() != Timer::RUNNING) {
        int8_t shift = 0;
        if (gModeSwitchBtn.click())
            shift = 1;
        else if (gModeSwitchBtn.pressing()) {
            shift = getEncoderDir();
        }

        if (shift) {
            gEncoder.clear();
            m_step = ADD_TO_ENUM(Step, m_step, shift);

            if (m_step == Step::setBeepVolume)
                gBeeper.start();
            else
                gBeeper.stop();

            gSettings.lagTime = m_lagTime;
            gSettings.updateEEPROM();
        }
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
    case Step::setViewInTests:
        processSetViewInTests();
        break;
    case Step::setViewInMasks:
        processSetViewInMasks();
        break;
    }
}

bool SettingsSetter::couldBeClosed() const {
    return m_timer.state() == Timer::STOPPED;
}
