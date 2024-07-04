#include "FStopTestMode.h"

#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 6, 5, 4, 3, 2, 1 };
} // namespace

FStopTestMode::FStopTestMode() {
    m_initTime = 20_ts;
    m_FStopPartId = 5;
    m_step = Step::initTime;
    m_currentRun = 0;
}

void FStopTestMode::switchMode() {
    m_step = (Step)(((int)m_step + 1) % (int)Step::last_);
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

    gDisplay[0] << "F Test#" << m_currentRun + 1 << " T:" << gTimer.total();

    gDisplay[1] >> "f 1/" >> kFStopPartVarinatns[m_FStopPartId];

    if (gTimer.state() == Timer::RUNNING) {
        gTimer.printFormatedState();
        return;
    }

    gDisplay[1] << getPrintTime();
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

void FStopTestMode::printLog(bool& requestExit) {
    gDisplay[0] << "F Log ";

    uint8_t id = printLogHelper(
        [](void* this__, uint8_t id, bool& current, bool& end) -> Time {
            auto this_ = reinterpret_cast<FStopTestMode*>(this__);
            float stopPart = kFStopPartVarinatns[this_->m_FStopPartId];

            current = this_->m_step == Step::run && this_->m_currentRun == id;
            return { this_->m_initTime * pow(2, id / stopPart) };
        },
        this);

    if (m_step == Step::run) {
        if (m_currentRun < id)
            processRun();
        else
            requestExit = true;
    }
}
