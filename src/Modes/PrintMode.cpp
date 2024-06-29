#include "PrintMode.h"

#include "../Tools.h"

PrintMode::PrintMode() {
    m_printTime = 80_ts;
    m_triggerByHold = false;
    gTimer.resetAfterLastResume();
}

void PrintMode::process() {
    if (gExtraBtn.click() && gTimer.state() != Timer::RUNNING) {
        m_triggerByHold = !m_triggerByHold;
    }

    gDisplay[0] << "Prnt " << (m_triggerByHold ? "HLD T:" : "CLK T:") << gTimer.total();

    if (gTimer.state() == Timer::STOPPED) {
        gTimer.resetTotal();
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
    gTimer.resetAfterLastResume();
}
