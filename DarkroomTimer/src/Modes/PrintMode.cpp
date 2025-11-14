#include "PrintMode.h"

#include "../Hardware.h"
#include "../Utils.h"

PrintMode::PrintMode() {
    m_timeTable.setBuffer(gModesCache, sizeof(gModesCache));
    m_timeTable.printBadAsZero(false);
    m_timeTable.reset();

    m_printTime = 8_s;
    m_triggerByHold = false;
}

void PrintMode::switchMode() {
    if (m_showLog)
        return;

    m_triggerByHold = !m_triggerByHold;
    repaint();
}

void PrintMode::process() {
    if (gEncoderBtn.click() && gTimer.state() != Timer::RUNNING) {
        gScrollableContent.goToFirstLine();
        m_showLog = !m_showLog;
        repaint();
        return;
    }

    if (m_showLog) {
        gScrollableContent.scroll(gEncoder.getDir());
        gScrollableContent.paint();
        return;
    }

    if (gTimer.justFinished()) {
        appendPrintLog(gTimer.afterLastResume());
        repaint();
    }

    switch (gTimer.state()) {
    case Timer::STOPPED:
        if (gEncoder.getTime(m_printTime))
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
            case Timer::RUNNING:
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
        m_timeTable.setPrefix("Log " kSheetSym);
        m_timeTable.flush();
        gScrollableContent.paint();
        return;
    }

    gDisplay[0] << F("Prnt ") << (m_triggerByHold ? F("HLD T:") : F("CLK T:")) << gTimer.total();

    switch (gTimer.state()) {
    case Timer::STOPPED:
        gDisplay[1] << F(kPlaySym " ") << m_printTime;
        break;
    case Timer::RUNNING:
        gDisplay[1] << F(kPlaySym " ");
        if (gTimer.lag())
            gDisplay[1] << F("Lag");
        else
            gDisplay[1] << Time::fromMillis(gTimer.left());
        break;
    case Timer::PAUSED:
        gDisplay[1] << F(kPauseSym " ");
        gDisplay[1] << Time::fromMillis(gTimer.left());
        break;
    }
    gDisplay[1] >> gTimer.afterLastResume();
}

void PrintMode::reset() {
    if (gTimer.state() == Timer::RUNNING)
        appendPrintLog(gTimer.afterLastResume());

    m_showLog = false;
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
