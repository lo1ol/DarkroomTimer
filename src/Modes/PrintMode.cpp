#include "PrintMode.h"

#include "../Tools.h"

PrintMode::PrintMode() {
    m_printTime = 8_s;
    m_triggerByHold = false;
    m_logSize = 0;
}

void PrintMode::switchMode() {
    m_triggerByHold = !m_triggerByHold;
}

void PrintMode::process() {
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

void PrintMode::reset() {
    appendPrintLog(gTimer.afterLastResume());
}

void PrintMode::resetPrintInfo() {
    gTimer.reset();
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
    gDisplay[0] << "Log ";

    printLogHelper(
        [](const void* this__, uint8_t id, bool& current, const char*& mark) -> Time {
            auto this_ = reinterpret_cast<const PrintMode*>(this__);

            if (id == this_->m_logSize)
                return kBadTime;

            return { this_->m_printLog[id] };
        },
        this);
}
