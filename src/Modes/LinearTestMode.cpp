#include "LinearTestMode.h"

#include "../Tools.h"

LinearTestMode::LinearTestMode() {
    m_initTime = 80_ts;
    m_stepTime = 20_ts;
    m_step = Step::initTime;
}

void LinearTestMode::process() {
    if (gModeSwitchBtn.click() && gTimer.state() != Timer::RUNNING) {
        m_step = (Step)(((int)m_step + 1) % (int)Step::last_);
        m_currentRun = 1;
        gTimer.resetTotal();
    }

    switch (m_step) {
    case Step::initTime:
        gDisplay[0] << "Linear tests";
        getTime(m_initTime);
        gDisplay[1] << "Init t:" << m_initTime;
        return;
    case Step::stepTime:
        gDisplay[0] << "Linear tests";
        getTime(m_stepTime);
        gDisplay[1] << "Step t:" << m_stepTime;
        return;
    case Step::run:
        break;
    }

    int run = m_currentRun - (gTimer.state() == Timer::RUNNING);
    gDisplay[0] << "L Test#" << run << " T:" << gTimer.total();

    if (gTimer.state() == Timer::RUNNING) {
        gTimer.printFormatedState();
        return;
    }

    Time printTime;
    if (m_currentRun == 1)
        printTime = m_initTime;
    else
        printTime = m_stepTime;

    gDisplay[1] << printTime;

    if (gStartBtn.click()) {
        if (gTimer.state() == Timer::STOPPED) {
            gTimer.start(printTime);
            ++m_currentRun;
        }
    }
}

void LinearTestMode::reset() {
    if (m_step != Step::run)
        return;

    gTimer.stop();
    gTimer.resetTotal();
    m_currentRun = 1;
}

void LinearTestMode::printLog() const {
    gDisplay[0] << "L Log ";
    uint8_t id = 0;

    for (uint8_t row = 0; row != DISPLAY_ROWS; ++row) {
        while (true) {
            char str[DISPLAY_COLS + 1] = { 0 };
            Time time = m_initTime + m_stepTime * id;
            time.getFormatedTime(str, false);
            if (!gDisplay[row].tryPrint(str))
                break;

            gDisplay[row] << " ";
            ++id;
        }
    }
}
