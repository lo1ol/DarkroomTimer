#include "PrintMode.h"

#include "../Tools.h"

PrintMode::PrintMode() {
    gTimeTable[0].printBadAsZero(false);
    gTimeTable[0].setPrefix("Log ");
    gTimeTable[0].empty();

    m_printTime = 8_s;
    m_triggerByHold = false;

    repaint();
}

void PrintMode::switchMode() {
    if (m_showLog) {
        m_showLog = false;
        repaint();
        return;
    }

    m_triggerByHold = !m_triggerByHold;
    repaint();
}

void PrintMode::process() {
    if (gExtraBtn.click() && gTimer.state() != Timer::RUNNING) {
        m_showLog = !m_showLog;
        repaint();
        return;
    }

    if (m_showLog) {
        gScrollableContent.scroll();
        gScrollableContent.paint();
        return;
    }

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
    if (m_showLog) {
        gScrollableContent.reset();
        gTimeTable[0].flush(true);
        gScrollableContent.paint();
        return;
    }
}

void PrintMode::reset() {
    appendPrintLog(gTimer.afterLastResume());
}

void PrintMode::resetPrintInfo() {
    gTimer.reset();
    gTimeTable[0].empty();
    m_logSize = 0;
    repaint();
}

void PrintMode::appendPrintLog(const Time& time) {
    if (time == 0_ts)
        return;

    if (m_logSize == TimeTable::kTimeTableSize)
        return;

    gTimeTable[0].setTime(m_logSize, time);
    ++m_logSize;
}
