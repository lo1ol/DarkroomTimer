#include "FStopTestMode.h"

#include "../DisplayLine.h"
#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 6, 5, 4, 3, 2, 1 };

// Ardruino cpp library haven't std::function,
// so it's pretty hard to pass lambda that capture something
// We need this vars inside so let's make them static objects
uint32_t gFStopInitTime;
uint8_t gFStopPartId;
} // namespace

FStopTestMode::FStopTestMode() {
    gFStopInitTime = 2 * 1000;
    gFStopPartId = 5;
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

        getTime(gFStopInitTime);
        gDisplay[1] << "Init t:" << gFStopInitTime;
        return;
    case Step::fstopSet: {
        gDisplay[0] << "Test F Stops";

        getInt(gFStopPartId, 0, sizeof(kFStopPartVarinatns) - 1);
        gDisplay[1] << "F stop: 1/" << kFStopPartVarinatns[gFStopPartId];
    }
        return;
    case Step::log:
        gDisplay.printTimeLog(
            "F Log ",
            [](uint8_t N) -> uint32_t {
                uint8_t stopPart = kFStopPartVarinatns[gFStopPartId];
                return lround((gFStopInitTime / 100) * pow(2, float(N) / stopPart)) * 100;
            },
            100);
        return;
    case Step::run:
        break;
    }

    int run = m_currentRun - (gTimer.state() == Timer::RUNNING);
    gDisplay[0] << "F Test#" << run << " T:" << gTimer.total();

    gDisplay[1] >> "f 1/" >> kFStopPartVarinatns[gFStopPartId];

    if (gTimer.state() == Timer::RUNNING) {
        gTimer.printFormatedState();
        return;
    }

    uint32_t printTime;
    if (m_currentRun == 1)
        printTime = gFStopInitTime;
    else {
        uint8_t stopPart = kFStopPartVarinatns[gFStopPartId];
        printTime =
            gFStopInitTime * (pow(2, float(m_currentRun - 1) / stopPart) - pow(2, float(m_currentRun - 2) / stopPart));
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
