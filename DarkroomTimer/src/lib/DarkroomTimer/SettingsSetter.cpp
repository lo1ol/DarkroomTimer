#include "SettingsSetter.h"

#include "DisplayLine.h"
#include "Tools.h"

namespace {
constexpr Time kMaxLagTime = 2_s;
} // namespace

SettingsSetter::SettingsSetter() : m_lagTime(gSettings.lagTime) {
    repaint();
}

SettingsSetter::~SettingsSetter() {
    gBeeper.stop();
    gSettings.lagTime = m_lagTime;
    gSettings.updateEEPROM();
}

void SettingsSetter::processSetLagTime() {
    gSettings.lagTime = 0_s;
    if (gEncoder.getTime(m_lagTime, true)) {
        if (m_lagTime > kMaxLagTime)
            m_lagTime = kMaxLagTime;
        repaint();
    }

    m_timer.tick();
    if (m_timer.state() != Timer::STOPPED) {
        repaint();
        return;
    }

    if (gStartBtn.click())
        m_timer.start(m_lagTime);
}

void SettingsSetter::processSetBeepVolume() const {
    uint8_t userVolume = (gSettings.beepVolume - MIN_BEEP_VOLUME) / BEEP_VOLUME_STEP;
    if (!gEncoder.getInt(userVolume, 0, 9))
        return;

    gBeeper.start();
    gSettings.beepVolume = MIN_BEEP_VOLUME + userVolume * BEEP_VOLUME_STEP;
    repaint();
}

void SettingsSetter::processSetAutoFinishView() const {
    if (gEncoder.getInt(gSettings.autoFinishViewMinutes, 0, 10))
        repaint();
}

void SettingsSetter::processSetBacklight() const {
    uint8_t userBacklight = min(gSettings.backlight, MAX_BACKLIGHT * 10) / MAX_BACKLIGHT;
    if (!gEncoder.getInt(userBacklight, 1, 10))
        return;
    gSettings.backlight = userBacklight * MAX_BACKLIGHT;
    gAnalogWrite(BACKLIGHT, gSettings.backlight);
    repaint();
}

void SettingsSetter::processStartWithSettings() const {
    uint8_t choice = gSettings.startWithSettings;
    if (!gEncoder.getInt(choice, 0, 1))
        return;

    gSettings.startWithSettings = choice;
    repaint();
}

void SettingsSetter::processSetMelody() const {
    uint8_t choice = gSettings.melody;

    if (!gEncoder.getInt(choice, 0, Melody::last_ - 1))
        return;

    gSettings.melody = static_cast<Melody::Name>(choice);
    gBeeper.setMelody(gSettings.melody);
    gBeeper.alarm();
    repaint();
}

void SettingsSetter::process() {
    if (m_timer.state() != Timer::RUNNING) {
        int8_t shift = 0;
        if (gModeBtn.click())
            shift = 1;
        else if (gModeBtn.pressing()) {
            if ((shift = gEncoder.getDir()))
                gModeBtn.skipEvents();
        }

        if (shift) {
            gEncoder.clear();
            m_step = ADD_TO_ENUM(Step, m_step, shift);

            gBeeper.stop();
            repaint();
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
    case Step::setMelody:
        processSetMelody();
        break;
    case Step::checkVersion:
        break;
    }
}

void SettingsSetter::repaint() const {
    gDisplay.reset();
    switch (m_step) {
    case Step::setLagTime:
        gDisplay[0] << "Lag time";
        gDisplay[1] << ((m_timer.state() != Timer::STOPPED) ? m_timer.afterLastResume() : m_lagTime);
        return;
    case Step::setBacklight:
        gDisplay[0] << "Backlight";
        gDisplay[1] << gSettings.backlight / MAX_BACKLIGHT;
        return;
    case Step::setBeepVolume:
        gDisplay[0] << "Beep volume";
        gDisplay[1] << ((gSettings.beepVolume - MIN_BEEP_VOLUME) / BEEP_VOLUME_STEP + 1);
        return;
    case Step::setAutoFinishView:
        gDisplay[0] << "Auto finish view";

        if (gSettings.autoFinishViewMinutes == 0)
            gDisplay[1] << "No";
        else
            gDisplay[1] << gSettings.autoFinishViewMinutes << " minute"
                        << (gSettings.autoFinishViewMinutes == 1 ? "" : "s");
        return;
    case Step::setStartWithSettings:
        gDisplay[0] << "Start with stngs";
        gDisplay[1] << (gSettings.startWithSettings ? "Yes" : "No");
        return;
    case Step::setMelody:
        gDisplay[0] << "Notify melody";
        gDisplay[1] << Melody::getMelodyName(gSettings.melody);
        return;
    case Step::checkVersion:
        gDisplay[0] << "Version";
        gDisplay[1] << TIMER_FIRMWARE_VERSION;
        return;
    }
}

bool SettingsSetter::couldBeClosed() const {
    return m_timer.state() == Timer::STOPPED;
}
