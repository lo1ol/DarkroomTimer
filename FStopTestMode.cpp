#include "FStopTestMode.h"

#include "Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 6, 5, 4, 3, 2, 1 };

// Ardruino cpp library haven't std::function,
// so it's pretty hard to pass lambda that capture something
// We need this vars inside so let's make them static objects
unsigned long gFStopInitTime;
size_t gFStopPartId;
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
        printFormatedLine("Test F Stops", 0);

        getTime(gFStopInitTime);
        printFormatedTime("Init t:", gFStopInitTime);
        return;
    case Step::fstopSet: {
        printFormatedLine("Test F Stops", 0);

        getInt(gFStopPartId, 0, sizeof(kFStopPartVarinatns) - 1);
        char str[MAX_SYMS_PER_LINE + 1];
        sprintf(str, "F stop: 1/%u", (unsigned)kFStopPartVarinatns[gFStopPartId]);
        printFormatedLine(str, 1);
    }
        return;
    case Step::log:
        printTimeLog(
            "F Log ",
            [](size_t N) -> unsigned long {
                uint8_t stopPart = kFStopPartVarinatns[gFStopPartId];
                return gFStopInitTime * (pow(2, double(N) / stopPart));
            },
            100);
        return;
    case Step::run:
        break;
    }

    char str[MAX_SYMS_PER_LINE + 1];
    char formatedTotal[5];
    getFormatedTime(gTimer.total(), formatedTotal);

    sprintf(str, "F Test#%d T:%s", m_currentRun - (gTimer.state() == Timer::RUNNING), formatedTotal);
    printFormatedLine(str, 0);

    if (gTimer.state() == Timer::RUNNING) {
        printFormatedTime("", gTimer.left());

        return;
    }

    unsigned long printTime;
    if (m_currentRun == 1)
        printTime = gFStopInitTime;
    else {
        uint8_t stopPart = kFStopPartVarinatns[gFStopPartId];
        printTime = gFStopInitTime *
                    (pow(2, double(m_currentRun - 1) / stopPart) - pow(2, double(m_currentRun - 2) / stopPart));
    }

    printFormatedTime("", printTime);

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
