#include "LinearTestMode.h"

#include "Tools.h"

LinearTestMode::LinearTestMode() {
    m_initTime = 8 * 1000;
    m_stepTime = 2 * 1000;
    m_step = Step::initTime;
}

void LinearTestMode::process() {
    if (gExtraBtn.press() && gTimer.state() != Timer::RUNNING) {
        m_step = (Step)(((int)m_step + 1) % (int)Step::last_);
        m_currentRun = 1;
        gTimer.resetTotal();
    }

    switch (m_step) {
    case Step::initTime:
        printFormatedLine("Linear tests", 0);
        getTime(m_initTime);
        printFormatedTime("Init t:", m_initTime);
        return;
    case Step::stepTime:
        printFormatedLine("Linear tests", 0);
        getTime(m_stepTime);
        printFormatedTime("Step t:", m_stepTime);
        return;
    case Step::run:
        break;
    }

    char str[MAX_SYMS_PER_LINE + 1];
    char formatedTotal[5];
    getFormatedTime(gTimer.total(), formatedTotal);

    sprintf(str, "L Test#%d T:%s", m_currentRun - (gTimer.state() == Timer::RUNNING), formatedTotal);
    printFormatedLine(str, 0);

    if (gTimer.state() == Timer::RUNNING) {
        printFormatedTime("", gTimer.left());
        return;
    }

    unsigned long printTime;
    if (m_currentRun == 1)
        printTime = m_initTime;
    else
        printTime = m_stepTime;

    printFormatedTime("", printTime);

    if (gStartBtn.press()) {
        if (gTimer.state() == Timer::STOPPED) {
            gTimer.start(printTime);
            ++m_currentRun;
        }
    }
}
