#include "PrintMode.h"

#include "../Tools.h"

PrintMode::PrintMode() {
    gTimeTable.printBadAsZero(false);
    gTimeTable.empty();

    m_printTime = 8_s;
    m_triggerByHold = false;

    repaint();
}

void PrintMode::switchMode() {
    m_triggerByHold = !m_triggerByHold;
    repaint();
}

void PrintMode::process() {
    gDisplay.reset();

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
        if (gStartBtn.press() && gTimer.state() == Timer::RUNNING) {
            if (gTimer.pause()) {
                appendPrintLog(gTimer.afterLastResume());
            }

            gStartBtn.skipEvents();
        }

        if (gStartBtn.click()) {
            switch (gTimer.state()) {
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

void PrintMode::repaint() const {
    // TODO
}

void PrintMode::reset() {
    appendPrintLog(gTimer.afterLastResume());
}

void PrintMode::resetPrintInfo() {
    gTimer.reset();
    gTimeTable.empty();
}

void PrintMode::appendPrintLog(const Time& time) {
    if (time == 0_ts)
        return;

    if (m_logSize == TimeTable::kTimeTableSize)
        return;

    gTimeTable.setTime(m_logSize, time);
    ++m_logSize;
}

void PrintMode::printLog() const {
    gTimeTable.paint();
}
