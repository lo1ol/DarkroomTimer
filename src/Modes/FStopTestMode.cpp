#include "FStopTestMode.h"

#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 6, 5, 4, 3, 2, 1 };
} // namespace

FStopTestMode::FStopTestMode() {
    m_initTime = 20_ts;
    m_FStopPartId = 5;
    m_step = Step::initTime;
    m_view = RunView::common;
    m_currentRun = 0;
}

void FStopTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    m_currentRun = 0;
    gTimer.reset();
}

void FStopTestMode::process() {
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

    switch (m_view) {
    case RunView::common:
        gDisplay[0] << "F Test#" << m_currentRun + 1 << " T:" << gTimer.total();

        gDisplay[1] >> "f 1/" >> kFStopPartVarinatns[m_FStopPartId];

        if (gTimer.state() == Timer::RUNNING) {
            gTimer.printFormatedState();
            return;
        }
        gDisplay[1] << getPrintTime();
        break;
    case RunView::log: {
        bool logOverFlow = false;
        printLog(logOverFlow);
        if (logOverFlow)
            m_view = RunView::common;
    } break;
    }

    processRun();
}

Time FStopTestMode::getPrintTime() const {
    if (m_currentRun == 0)
        return m_initTime;

    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * (pow(2, m_currentRun / stopPart) - pow(2, (m_currentRun - 1) / stopPart));
}

void FStopTestMode::processRun() {
    if (gTimer.stopped())
        ++m_currentRun;

    if (!gStartBtn.click())
        return;

    if (gTimer.state() != Timer::STOPPED)
        return;

    gTimer.start(getPrintTime());
}

void FStopTestMode::reset() {
    m_currentRun = 0;
}

void FStopTestMode::switchView() {
    m_view = ADD_TO_ENUM(RunView, m_view, 1);
}

bool FStopTestMode::canSwitchView() const {
    if (m_step != Step::run)
        return false;

    bool overFlow = false;
    printLog(overFlow);
    gDisplay.reset();
    return !overFlow;
}

void FStopTestMode::printLog(bool& logOverFlowed) const {
    gDisplay[0] << "F Log ";

    uint8_t id = printLogHelper(
        [](const void* this__, uint8_t id, bool& current, bool& end) -> Time {
            auto this_ = reinterpret_cast<const FStopTestMode*>(this__);
            float stopPart = kFStopPartVarinatns[this_->m_FStopPartId];

            current = this_->m_step == Step::run && this_->m_currentRun == id;
            return { this_->m_initTime * pow(2, id / stopPart) };
        },
        this);

    if (m_step == Step::run && m_currentRun >= id)
        logOverFlowed = true;
}
