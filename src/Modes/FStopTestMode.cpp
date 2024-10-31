#include "FStopTestMode.h"

#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 6, 5, 4, 3, 2, 1 };
} // namespace

FStopTestMode::FStopTestMode(bool splitGrade) : kSplit(splitGrade) {
    m_baseTime = 2_s;
    m_initTime = 2_s;
    m_FStopPartId = 5;
    m_step = kSplit ? Step::baseTime : Step::initTime;
    m_currentRun = kSplit ? 0 : 1;

    repaint();
}

void FStopTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    if (m_step == Step::baseTime && !kSplit)
        m_step = Step::initTime;

    m_currentRun = kSplit ? 0 : 1;
    gTimer.reset();

    repaint();
}

void FStopTestMode::process() {
    switch (m_step) {
    case Step::baseTime:
        if (getTime(m_baseTime))
            repaint();
        return;
    case Step::initTime:
        if (getTime(m_initTime))
            repaint();
        return;
    case Step::fstopSet:
        if (getInt(m_FStopPartId, 0, sizeof(kFStopPartVarinatns) - 1))
            repaint();
        return;
    case Step::run:
        break;
    }

    if (gTimer.state() == Timer::RUNNING)
        repaint();

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && getStepTotalTime(m_currentRun) != kBadTime)
        gTimer.start(getPrintTime());

    if (gTimer.stopped()) {
        if (m_currentRun == 0) { // don't take into account base time
            gTimer.reset();
            gBeeper.alarm("Change filter");
        }
        ++m_currentRun;
        repaint();
    }
}

void FStopTestMode::repaint() const {
    gDisplay.reset();

    switch (m_step) {
    case Step::baseTime:
        gDisplay[0] << preview();
        gDisplay[1] << "Base t:" << m_baseTime;
        return;
    case Step::initTime:
        gDisplay[0] << preview();
        gDisplay[1] << "Init t:" << m_initTime;
        return;
    case Step::fstopSet:
        gDisplay[0] << preview();
        gDisplay[1] << "F stop: 1/" << kFStopPartVarinatns[m_FStopPartId];
        return;
    case Step::run:
        gDisplay[0] << "Run ";
        printTimes();
        return;
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

Time FStopTestMode::getStepTotalTime(uint8_t id) const {
    if (id == 0)
        return m_baseTime;

    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * pow(2, (id - 1) / stopPart);
}

void FStopTestMode::reset() {
    m_currentRun = kSplit ? 0 : 1;
}

void FStopTestMode::printTimes() const {
    printTimeHelper(
        [](const void* this__, uint8_t id, bool& current, const char*& mark) -> Time {
            auto this_ = reinterpret_cast<const FStopTestMode*>(this__);

            if (!this_->kSplit)
                ++id;

            current = this_->m_step == Step::run && this_->m_currentRun == id;

            if (id == 0)
                mark = "ntf";

            return this_->getStepTotalTime(id);
        },
        this);
}

const char* FStopTestMode::preview() const {
    if (kSplit)
        return "Splt F Stop test";
    return "F Stop test";
}
