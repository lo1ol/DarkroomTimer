#include "PrintMode.h"

#include "Tools.h"

PrintMode::PrintMode() {
    m_printTime = 8 * 1000;
    m_triggerByHold = false;
}

void PrintMode::process() {
    if (gExtraBtn.click() && gTimer.state() != Timer::RUNNING) {
        m_triggerByHold = !m_triggerByHold;
    }

    char str[MAX_SYMS_PER_LINE + 1] = "Prnt ";
    concat(str, m_triggerByHold ? "HLD T=" : "CLK T=");
    concatTime(str, gTimer.total());
    printFormatedLine(str, 0);

    if (gTimer.state() == Timer::STOPPED) {
        gTimer.resetTotal();
        getTime(m_printTime);
        printFormatedTime("", m_printTime);
    } else {
        gTimer.printFormatedState();
    }

    if (!m_triggerByHold) {
        if (gStartBtn.press()) {
            switch (gTimer.state()) {
            case Timer::RUNNING:
                gTimer.pause();
                break;
            case Timer::PAUSED:
                gTimer.resume();
                break;
            case Timer::STOPPED:
                gTimer.start(m_printTime);
                break;
            }
        }

        return;
    }

    if (gStartBtn.press() && gTimer.state() == Timer::STOPPED)
        gTimer.start(m_printTime);

    if (gStartBtn.pressing())
        gTimer.resume();
    else
        gTimer.pause();
}

void PrintMode::reset() {
    gTimer.stop();
    gTimer.resetTotal();
}
