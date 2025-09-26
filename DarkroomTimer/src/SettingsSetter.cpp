#include "SettingsSetter.h"

#include "Hardware.h"
#include "Utils.h"

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
    if (m_timer.state() != Timer::STOPPED || m_timer.justFinished()) {
        repaint();
        return;
    }

    if (gStartBtn.click())
        m_timer.start(m_lagTime);
}

void SettingsSetter::processSetBeepVolume() const {
    if (!gEncoder.getInt(gSettings.beepVolume, kMinBeepVolume, kMaxBeepVolume))
        return;

    gBeeper.start();
    repaint();
}

void SettingsSetter::processSetAutoFinishView() const {
    if (gEncoder.getInt(gSettings.autoFinishViewMinutes, 0, kMaxAutoFinishViewMinutes))
        repaint();
}

void SettingsSetter::processSetBacklight() const {
    if (!gEncoder.getInt(gSettings.backlight, kMinBacklight, kMaxBacklight))
        return;
    setDisplayBacklight(gSettings.backlight);
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

            repaint();
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
    case Step::setMelody:
        processSetMelody();
        break;
    case Step::checkVersion:
        break;
    case Step::last_:
        assert(false);
        break;
    }
}

void SettingsSetter::repaint() const {
    gDisplay.reset();
    switch (m_step) {
    case Step::setLagTime:
        gDisplay[0] << "Lag time";
        gDisplay[1] << ((m_timer.state() != Timer::STOPPED) ? Time::fromMillis(m_timer.left()) : m_lagTime);
        return;
    case Step::setBacklight:
        gDisplay[0] << "Backlight";
        gDisplay[1] << gSettings.backlight;
        return;
    case Step::setBeepVolume:
        gDisplay[0] << "Beep volume";
        gDisplay[1] << gSettings.beepVolume;
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
    case Step::last_:
        assert(false);
        return;
    }
}

bool SettingsSetter::couldBeClosed() const {
    return m_timer.state() == Timer::STOPPED;
}
