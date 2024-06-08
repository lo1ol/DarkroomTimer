#include "PrintMode.h"

#include "Tools.h"

PrintMode::PrintMode() {
    m_printTime = 8 * 1000;
}

void PrintMode::process() {
    char str[MAX_SYMS_PER_LINE + 1];
    char formatedTime[5];

    getFormatedTime(gTimer.total(), formatedTime);

    sprintf(str, "Print: T=%s", formatedTime);
    printFormatedLine(str, 0);

    if (gTimer.state() == Timer::STOPPED) {
        gTimer.resetTotal();
        getTime(m_printTime);
        printFormatedTime("", m_printTime);
    } else {
        getFormatedTime(gTimer.left(), formatedTime);
        if (gTimer.state() == Timer::PAUSED)
            sprintf(str, "%s PAUSE", formatedTime);
        else
            sprintf(str, "%s", formatedTime);
        printFormatedLine(str, 1);
    }

    if (gExtraBtn.press()) {
        gTimer.stop();
        gTimer.resetTotal();
    }

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
}
