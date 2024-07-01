#include "PrintMode.h"

#include "../Tools.h"

PrintMode::PrintMode() {
    m_printTime = 80_ts;
    m_triggerByHold = false;
    resetPrintInfo();
}

void PrintMode::process() {
    if (gExtraBtn.click() && gTimer.state() != Timer::RUNNING) {
        m_triggerByHold = !m_triggerByHold;
    }

    gDisplay[0] << "Prnt " << (m_triggerByHold ? "HLD T:" : "CLK T:") << gTimer.total();

    if (gTimer.stopped())
        appendPrintLog(gTimer.afterLastResume());

    if (gTimer.state() == Timer::STOPPED) {
        getTime(m_printTime);
        gDisplay[1] << m_printTime;
    } else {
        gTimer.printFormatedState();
    }
    gDisplay[1] >> gTimer.afterLastResume();

    if (!m_triggerByHold) {
        if (gStartBtn.press()) {
            switch (gTimer.state()) {
            case Timer::RUNNING:
                if (gTimer.pause())
                    appendPrintLog(gTimer.afterLastResume());
                break;
            case Timer::PAUSED:
                gTimer.resume();
                break;
            case Timer::STOPPED:
                resetPrintInfo();
                gTimer.start(m_printTime);
                break;
            }
        }

        return;
    }

    if (gStartBtn.press() && gTimer.state() == Timer::STOPPED) {
        resetPrintInfo();
        gTimer.start(m_printTime);
    }
    if (gStartBtn.pressing())
        gTimer.resume();
    else {
        if (gTimer.pause())
            appendPrintLog(gTimer.afterLastResume());
    }
}

void PrintMode::reset() {
    gTimer.stop();
    appendPrintLog(gTimer.afterLastResume());
    gTimer.resetTotal();
    gTimer.resetAfterLastResume();
}

void PrintMode::resetPrintInfo() {
    gTimer.resetTotal();
    gTimer.resetAfterLastResume();
    m_logSize = 0;
}

void PrintMode::appendPrintLog(const Time& time) {
    if (time == 0_ts)
        return;

    if (m_logSize == sizeof(m_printLog) / sizeof(m_printLog[0]))
        return;

    m_printLog[m_logSize] = time;
    ++m_logSize;
}

void PrintMode::printLog() const {
    gDisplay[0] << "P Log ";
    uint8_t id = 0;

    for (uint8_t row = 0; row != DISPLAY_ROWS; ++row) {
        while (true) {
            if (id == m_logSize)
                break;

            char str[DISPLAY_COLS + 1] = { 0 };
            Time time = m_printLog[id];
            time.getFormatedTime(str);
            if (!gDisplay[row].tryPrint(str))
                break;

            gDisplay[row] << " ";
            ++id;
        }
    }
}
