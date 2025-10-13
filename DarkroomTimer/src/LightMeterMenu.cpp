#include "LightMeterMenu.h"

#include <Arduino.h>
#include "CustomLcdSyms.h"
#include "Hardware.h"

#define SLAVE_HANDLE_TIME_US 600

namespace Comm = Communication;

bool LightMeterMenu::askStart() {
    Comm::LmCmd cmd;
    if (m_con.readData(cmd)) {
        if (cmd == Comm::LmCmd::askStart) {
            m_lastSlaveResponseTime = gMillis();
            m_needRepaint = true;
            return true;
        }

        m_con.resetReadData();
    }

    return false;
}

void LightMeterMenu::process() {
    if (gModeBtn.click())
        m_bwOnly = !m_bwOnly;

    m_con.sendData(Comm::MainCmd::sendColorInfo);
    // all commands requires some time to be handled
    delayMicroseconds(SLAVE_HANDLE_TIME_US);

    if (m_con.readData(m_lightDesc)) {
        m_lastSlaveResponseTime = gMillis();
        m_needRepaint = true;
    } else {
        m_con.resetReadData();
    }

    if (!m_needRepaint)
        return;
    m_needRepaint = false;

    gDisplay.reset();
    gDisplay[0] << "LM ";
    if (m_lightDesc.abs)
        gDisplay[0] << "ABS ";
    else
        gDisplay[0] << "REL ";

    if (m_bwOnly)
        gDisplay[0] << "B&W ";
    else
        gDisplay[0] << "COL ";

    gDisplay[0].print3DigSignInt(m_lightDesc.lm, kWplusSym, kWminusSym);

    if (m_bwOnly)
        return;

    gDisplay[1].print3DigSignInt(m_lightDesc.y, kYplusSym, kYminusSym);
    gDisplay[1] << " ";
    gDisplay[1].print3DigSignInt(m_lightDesc.m, kMplusSym, kMminusSym);
    gDisplay[1] << " ";
    gDisplay[1].print3DigSignInt(m_lightDesc.c, kCplusSym, kCminusSym);
}

bool LightMeterMenu::needExit() {
    return (gMillis() - m_lastSlaveResponseTime > 2000) || gEncoderBtn.hold();
}

void LightMeterMenu::exit() {
    m_lightDesc = Comm::ColorDesc{};
    m_con.sendData(Comm::MainCmd::end);
    // all commands requires some time to be handled
    delayMicroseconds(SLAVE_HANDLE_TIME_US);
}
