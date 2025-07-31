#include "PrintMode.h"

#include "../Tools.h"

PrintMode::PrintMode() {
    m_timeTable.setBuffer(gModesCache, sizeof(gModesCache));
    m_timeTable.printBadAsZero(false);
    m_timeTable.setPrefix("Log");
    m_timeTable.reset();

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

    if (gTimer.stopped()) {
        appendPrintLog(gTimer.afterLastResume());
        repaint();
    }

    switch (gTimer.state()) {
    case Timer::STOPPED:
        if (getTime(m_printTime))
            repaint();
        break;
    case Timer::RUNNING:
        repaint();
        break;
    case Timer::PAUSED:
        break;
    }

    if (!m_triggerByHold) {
        if (gStartBtn.press() && gTimer.state() == Timer::RUNNING) {
            if (gTimer.pause()) {
                appendPrintLog(gTimer.afterLastResume());
            }

            gStartBtn.skipEvents();
            repaint();
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
            repaint();
        }

        return;
    }

    if (gStartBtn.press() && gTimer.state() == Timer::STOPPED) {
        resetPrintInfo();
        gTimer.start(m_printTime);
    }
    if (gStartBtn.pressing()) {
        gTimer.resume();
    } else {
        if (gTimer.pause())
            appendPrintLog(gTimer.afterLastResume());
    }
    repaint();
}

void PrintMode::repaint() {
    gDisplay.reset();

    if (m_showLog) {
        gScrollableContent.reset();
        m_timeTable.flush(true);
        gScrollableContent.paint();
        return;
    }

    gDisplay[0] << "Prnt " << (m_triggerByHold ? "HLD T:" : "CLK T:") << gTimer.total();

    switch (gTimer.state()) {
    case Timer::STOPPED:
        gDisplay[1] << m_printTime;
        break;
    case Timer::RUNNING:
        if (gTimer.lag())
            gDisplay[1] << "Lag";
        else
            gDisplay[1] << Time::fromMillis(gTimer.left());
        break;
    case Timer::PAUSED:
        gDisplay[1] << Time::fromMillis(gTimer.left()) << " PAUSE";
        break;
    }
    gDisplay[1] >> gTimer.afterLastResume();
}

void PrintMode::reset() {
    appendPrintLog(gTimer.afterLastResume());
}

void PrintMode::resetPrintInfo() {
    gTimer.reset();
    m_timeTable.reset();
    m_logSize = 0;
    repaint();
}

void PrintMode::appendPrintLog(const Time& time) {
    if (time == 0_ts)
        return;

    if (m_logSize == m_timeTable.capacity())
        return;

    m_timeTable.setTime(m_logSize, time);
    ++m_logSize;
}
