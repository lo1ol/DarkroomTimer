#include "FStopTestMode.h"

#include "../Config.h"
#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 6, 5, 4, 3, 2, 1 };
} // namespace

FStopTestMode::FStopTestMode() {
    m_initTime = 20_ts;
    m_FStopPartId = 5;
    m_step = Step::initTime;
}

void FStopTestMode::process() {
    if (gExtraBtn.click() && gTimer.state() != Timer::RUNNING) {
        m_step = (Step)(((int)m_step + 1) % (int)Step::last_);
        m_currentRun = 1;
        gTimer.resetTotal();
    }

    switch (m_step) {
    case Step::initTime:
        gDisplay[0] << "Test F Stops";

        getTime(m_initTime);
        gDisplay[1] << "Init t:" << m_initTime;
        return;
    case Step::fstopSet: {
        gDisplay[0] << "Test F Stops";

        getInt(m_FStopPartId, 0, sizeof(kFStopPartVarinatns) - 1);
        gDisplay[1] << "F stop: 1/" << kFStopPartVarinatns[m_FStopPartId];
    }
        return;
    case Step::run:
        break;
    }

    int run = m_currentRun - (gTimer.state() == Timer::RUNNING);
    gDisplay[0] << "F Test#" << run << " T:" << gTimer.total();

    gDisplay[1] >> "f 1/" >> kFStopPartVarinatns[m_FStopPartId];

    if (gTimer.state() == Timer::RUNNING) {
        gTimer.printFormatedState();
        return;
    }

    Time printTime;
    if (run == 1)
        printTime = m_initTime;
    else {
        float stopPart = kFStopPartVarinatns[m_FStopPartId];
        printTime = m_initTime * (pow(2, (run - 1) / stopPart) - pow(2, (run - 2) / stopPart));
    }

    gDisplay[1] << printTime;

    if (gStartBtn.click()) {
        if (gTimer.state() == Timer::STOPPED) {
            gTimer.start(printTime);
            ++m_currentRun;
        }
    }
}

void FStopTestMode::reset() {
    if (m_step != Step::run)
        return;

    gTimer.stop();
    gTimer.resetTotal();
    m_currentRun = 1;
}

void FStopTestMode::printLog() const {
    gDisplay[0] << "F Log ";
    uint8_t id = 0;
    float stopPart = kFStopPartVarinatns[m_FStopPartId];

    for (uint8_t row = 0; row != DISPLAY_ROWS; ++row) {
        while (true) {
            char str[DISPLAY_COLS + 1] = { 0 };
            Time time = m_initTime * pow(2, id / stopPart);
            time.getFormatedTime(str, false);
            if (!gDisplay[row].tryPrint(str))
                break;

            gDisplay[row] << " ";
            ++id;
        }
    }
}
