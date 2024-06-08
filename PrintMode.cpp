#include "PrintMode.h"

#include "Tools.h"

PrintMode::PrintMode() {
    printFormatedLine("Print", 0);
    m_printTime = 8 * 1000;
}

void PrintMode::process() {
    if (gTimer.state() == Timer::STOPPED) {
        getTime(m_printTime);
        printFormatedTime("", m_printTime);
    } else {
        printFormatedTime("", gTimer.left());
    }

    if (gExtraBtn.pressed())
        gTimer.stop();

    if (gStartBtn.pressed()) {
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
