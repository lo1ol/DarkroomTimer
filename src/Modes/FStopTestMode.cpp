#include "FStopTestMode.h"

#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 12, 6, 4, 3, 2, 1 };
} // namespace

FStopTestMode::FStopTestMode(bool splitGrade) : kSplit(splitGrade) {
    gTimeTable[0].printBadAsZero(false);

    m_baseTime = 2_s;
    m_initTime = 2_s;
    m_FStopPartId = 5;
    m_step = kSplit ? Step::baseTime : Step::initTime;
    m_currentRun = 0;

    repaint();
}

void FStopTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    if (m_step == Step::baseTime && !kSplit)
        m_step = Step::initTime;

    m_currentRun = 0;
    if (m_step == Step::run) {
        setTimeTable();
        gTimeTable[0].setCurrent(0, kSplit ? "ntf" : nullptr);
    }

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

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && getStepTotalTime(m_currentRun) != kBadTime &&
        gScrollableContent.currentIsPrinted())
        gTimer.start(getPrintTime());

    if (gTimer.stopped()) {
        if (m_currentRun == 0 && kSplit) { // don't take into account base time
            gTimer.reset();
            gBeeper.alarm("Change filter");
        }
        gTimeTable[0].setCurrent(++m_currentRun);
        repaint();
    }

    if (gTimer.state() == Timer::STOPPED)
        gScrollableContent.scroll();

    gScrollableContent.paint();
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
        gScrollableContent.reset();
        gTimeTable[0].flush(true);
        gScrollableContent.paint();
        return;
    }
}

Time FStopTestMode::getPrintTime() const {
    uint8_t realStep = m_currentRun;
    if (!kSplit)
        ++realStep;

    if (realStep == 0)
        return m_baseTime;

    if (realStep == 1)
        return m_initTime;

    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * (pow(2, (realStep - 1) / stopPart) - pow(2, (realStep - 2) / stopPart));
}

Time FStopTestMode::getStepTotalTime(uint8_t id) const {
    uint8_t realId = id;
    if (!kSplit)
        ++realId;

    if (realId == 0)
        return m_baseTime;

    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * pow(2, (realId - 1) / stopPart);
}

void FStopTestMode::reset() {
    m_currentRun = 0;
    gTimeTable[0].setCurrent(0, kSplit ? "ntf" : nullptr);
    repaint();
}

void FStopTestMode::setTimeTable() const {
    gTimeTable[0].empty();
    gTimeTable[0].setPrefix("Run ");
    uint8_t id = 0;
    while (true) {
        if (id == TimeTable::kTimeTableSize)
            break;
        Time time = getStepTotalTime(id);
        if (time == kBadTime)
            break;
        gTimeTable[0].setTime(id++, time);
    }
}

const char* FStopTestMode::preview() const {
    if (kSplit)
        return "Splt F Stop test";
    return "F Stop test";
}

