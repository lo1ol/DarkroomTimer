#include "LightMeterMenu.h"

#include <Arduino.h>
#include "Hardware.h"
#include "Utils.h"

#define ASK_TIME_FREQ_MS 300

namespace Comm = Communication;

LightMeterMenu::LightMeterMenu() {
    gConnection.start();
}

bool LightMeterMenu::askStart() {
    Comm::LmCmd cmd;
    if (gConnection.readData(cmd)) {
        if (cmd == Comm::LmCmd::askStart) {
            gConnection.resetError();
            syncLightmeter();
            gDigitalWrite(RELAY_PIN, true);
            m_lightMeasure = {};
            m_menu = Menu::ShowLightInfo;
            return true;
        }

        gConnection.resetReadData();
    }

    return false;
}

void LightMeterMenu::process() {
    if (gViewBtn.click()) {
        m_lastUserCommunicationTime = gMillis();
        m_needRepaint = true;
        m_settingBaseLogD = true;
        m_lastAskTime = gMillis() - ASK_TIME_FREQ_MS;

        if (m_menu == Menu::SetBaseTime) {
            gConnection.sendCmdWithData(Comm::MainCmd::setBaseTime, m_baseTime);
            delay(25); // writing to EEPROM requires extra 20ms
            m_menu = Menu::ShowLightInfo;
        } else {
            gConnection.sendCmd(Comm::MainCmd::sendBaseTime);
            if (!gConnection.readData(m_baseTime))
                return;

            m_menu = Menu::SetBaseTime;
        }
    }

    switch (m_menu) {
    case Menu::ShowLightInfo:
        m_needRepaint = true;
        handleShowLightInfo();
        break;
    case Menu::SetBaseTime:
        handleSetBaseTime();
        break;
    }

    repaint();
}

void LightMeterMenu::handleSetBaseTime() {
    if (gEncoderBtn.click()) {
        m_lastUserCommunicationTime = gMillis();
        m_needRepaint = true;
        gDisplay.resetBlink(true);
        m_settingBaseLogD = !m_settingBaseLogD;
        return;
    }

    if (m_settingBaseLogD)
        m_needRepaint |= gEncoder.getInt(m_baseTime.baseLogD, 0, 999);
    else
        m_needRepaint |= gEncoder.getTime(m_baseTime.baseTime);

    if (m_needRepaint) {
        m_lastUserCommunicationTime = gMillis();
        gDisplay.resetBlink(false);
    }
}

void LightMeterMenu::handleShowLightInfo() {
    if (gModeBtn.click()) {
        m_bwOnly = !m_bwOnly;
        syncLightmeter();
    }

    if (gMillis() - m_lastAskTime < ASK_TIME_FREQ_MS)
        return;

    bool prevAbs = m_lightMeasure.abs;

    m_lastAskTime = gMillis();
    gConnection.sendCmd(Comm::MainCmd::sendLightMeasure);
    if (!gConnection.readData(m_lightMeasure))
        return;

    if (prevAbs != m_lightMeasure.abs)
        m_lastUserCommunicationTime = gMillis();

    m_needRepaint = true;
}

void LightMeterMenu::repaint() {
    if (!m_needRepaint)
        return;
    m_needRepaint = false;
    gDisplay.reset();

    switch (m_menu) {
    case Menu::ShowLightInfo:
        repaintShowLightInfo();
        break;
    case Menu::SetBaseTime:
        repaintSetBaseTime();
        break;
    }
}

void LightMeterMenu::repaintSetBaseTime() {
    gDisplay[0] << "Base logD:";

    char buf[DISPLAY_COLS + 1];
    itoa(m_baseTime.baseLogD, buf, 10);
    alignStr(buf, 3, '0');
    gDisplay[0].print(buf, m_settingBaseLogD);

    gDisplay[1] << "Base T:";
    m_baseTime.baseTime.getFormatedTime(buf);
    gDisplay[1].print(buf, !m_settingBaseLogD);
}

void LightMeterMenu::repaintShowLightInfo() {
    gDisplay[0] << "LM ";
    if (m_bwOnly)
        gDisplay[0] << "B&W";
    else
        gDisplay[0] << "Color";

    if (m_lightMeasure.abs)
        gDisplay[0] << " T " << m_lightMeasure.suggestedTime;

    if (m_bwOnly) {
        gDisplay[1] << "logD";

        if (!m_lightMeasure.abs) {
            gDisplay[1].print3DigInt(m_lightMeasure.lightDesc.logD, '+', '-');
            gDisplay[1] << " REL";
        } else {
            gDisplay[1] << " ";
            gDisplay[1].print3DigInt(m_lightMeasure.lightDesc.logD, 0, 0);
        }

        return;
    }

    gDisplay[1] << "D";
    gDisplay[1].print3DigInt(m_lightMeasure.lightDesc.logD, 0, 0);
    gDisplay[1] << " Y";
    gDisplay[1].print3DigInt(m_lightMeasure.lightDesc.logBlue, '+', '-');
    gDisplay[1] << " M";
    gDisplay[1].print3DigInt(m_lightMeasure.lightDesc.logGreen, '+', '-');
}

bool LightMeterMenu::needExit() {
    return (gMillis() - m_lastUserCommunicationTime) > gSettings.autoFinishViewMinutes * 60000UL ||
           gConnection.hasError();
}

void LightMeterMenu::exit() {
    gConnection.sendCmd(Comm::MainCmd::end);
    gDigitalWrite(RELAY_PIN, false);
}

void LightMeterMenu::syncLightmeter() {
    gConnection.sendCmd(m_bwOnly ? Comm::MainCmd::switchBW : Comm::MainCmd::switchColor);
    gConnection.sendCmd(Comm::MainCmd::sendBaseTime);
    if (!gConnection.readData(m_baseTime))
        return;

    m_lastAskTime = gMillis() - ASK_TIME_FREQ_MS;
    m_needRepaint = true;
    m_lastUserCommunicationTime = gMillis();
}
