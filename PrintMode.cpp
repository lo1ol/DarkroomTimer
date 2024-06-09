#include "PrintMode.h"

#include "Tools.h"

PrintMode::PrintMode() {
    m_printTime = 8 * 1000;
}

void PrintMode::process() {
    char str[MAX_SYMS_PER_LINE + 1] = "Print: T=";
    concatTime(str, gTimer.total());
    printFormatedLine(str, 0);

    if (gTimer.state() == Timer::STOPPED) {
        gTimer.resetTotal();
        getTime(m_printTime);
        printFormatedTime("", m_printTime);
    } else {
        gTimer.printFormatedState();
    }

    if (gStartBtn.click()) {
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
}

void PrintMode::reset() {
    gTimer.stop();
    gTimer.resetTotal();
}
