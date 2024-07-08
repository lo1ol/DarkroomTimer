#include "FStopTestMode.h"

#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 6, 5, 4, 3, 2, 1 };
} // namespace

FStopTestMode::FStopTestMode(bool splitGrade) : kSplit(splitGrade) {
    m_baseTime = 20_ts;
    m_initTime = 20_ts;
    m_FStopPartId = 5;
    m_step = kSplit ? Step::baseTime : Step::initTime;
    m_view = gSettings.logViewInTests ? RunView::log : RunView::common;
    m_currentRun = kSplit ? 0 : 1;
}

void FStopTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    if (m_step == Step::baseTime && !kSplit)
        m_step = Step::initTime;

    m_currentRun = kSplit ? 0 : 1;
    gTimer.reset();
}

void FStopTestMode::process() {
    switch (m_step) {
    case Step::baseTime:
        gDisplay[0] << preview();

        getTime(m_baseTime);
        gDisplay[1] << "Base t:" << m_baseTime;
        return;
    case Step::initTime:
        gDisplay[0] << preview();

        getTime(m_initTime);
        gDisplay[1] << "Init t:" << m_initTime;
        return;
    case Step::fstopSet: {
        gDisplay[0] << preview();

        getInt(m_FStopPartId, 0, sizeof(kFStopPartVarinatns) - 1);
        gDisplay[1] << "F stop: 1/" << kFStopPartVarinatns[m_FStopPartId];
    }
        return;
    case Step::run:
        break;
    }

    switch (m_view) {
    case RunView::common:
        if (m_currentRun == 0)
            gDisplay[0] << "Base printing";
        else
            gDisplay[0] << "Test #" << m_currentRun << " T:" << gTimer.total();

        gDisplay[1] >> "f 1/" >> kFStopPartVarinatns[m_FStopPartId];

        if (gTimer.state() == Timer::RUNNING) {
            gTimer.printFormatedState();
            return;
        }
        gDisplay[1] << getPrintTime();
        break;
    case RunView::log: {
        gDisplay[0] << "Run ";

        bool logOverFlow = false;
        printLog(logOverFlow);
        if (logOverFlow)
            m_view = RunView::common;
    } break;
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click())
        gTimer.start(getPrintTime());

    if (gTimer.stopped()) {
        if (m_currentRun == 0) { // don't take into account base time
            gTimer.reset();
            gBeeper.melody();
        }
        ++m_currentRun;
    }
}

Time FStopTestMode::getPrintTime() const {
    if (m_currentRun == 0)
        return m_baseTime;

    if (m_currentRun == 1)
        return m_initTime;

    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * (pow(2, (m_currentRun - 1) / stopPart) - pow(2, (m_currentRun - 2) / stopPart));
}

void FStopTestMode::reset() {
    m_currentRun = kSplit ? 0 : 1;
}

void FStopTestMode::switchView() {
    m_view = ADD_TO_ENUM(RunView, m_view, 1);
}

bool FStopTestMode::canSwitchView() const {
    if (m_step != Step::run)
        return false;

    gDisplay[0] << "Run ";
    bool overFlow = false;
    printLog(overFlow);
    gDisplay.reset();
    return !overFlow;
}

void FStopTestMode::printLog(bool& logOverFlowed) const {
    uint8_t id = printLogHelper(
        [](const void* this__, uint8_t id, bool& current, bool& end, const char*& mark) -> Time {
            auto this_ = reinterpret_cast<const FStopTestMode*>(this__);
            float stopPart = kFStopPartVarinatns[this_->m_FStopPartId];

            if (!this_->kSplit)
                ++id;

            current = this_->m_step == Step::run && this_->m_currentRun == id;

            if (id == 0) {
                mark = "ntf";
                return this_->m_baseTime;
            }

            return { this_->m_initTime * pow(2, (id - 1) / stopPart) };
        },
        this);

    if (!kSplit)
        ++id;

    if (m_step == Step::run && m_currentRun >= id)
        logOverFlowed = true;
}

void FStopTestMode::printLog() const {
    gDisplay[0] << "Log ";

    bool unused;
    printLog(unused);
}

const char* FStopTestMode::preview() const {
    if (kSplit)
        return "Splt F Stop test";
    return "F Stop test";
}
