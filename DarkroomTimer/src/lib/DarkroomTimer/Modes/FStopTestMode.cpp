#include "FStopTestMode.h"

#include "../Tools.h"

namespace {
constexpr uint8_t kFStopPartVarinatns[] = { 12, 6, 4, 3, 2, 1 };
} // namespace

FStopTestMode::FStopTestMode(SubMode subMode) : kSubMode(subMode) {
    m_timeTable.setBuffer(gModesCache, sizeof(gModesCache));
    m_timeTable.printBadAsZero(false);

    m_baseTime = 2_s;
    m_initTime = 2_s;
    m_FStopPartId = 5;
    m_step = kSubMode == SplitGrade ? Step::baseTime : Step::initTime;
    m_currentRun = 0;

    repaint();
}

void FStopTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    if (m_step == Step::baseTime && kSubMode != SplitGrade)
        m_step = Step::initTime;

    m_currentRun = 0;
    if (m_step == Step::run) {
        setTimeTable();
        m_timeTable.setCurrent(0, kSubMode == SplitGrade ? "ntf" : nullptr);
    }

    gTimer.reset();

    repaint();
}

void FStopTestMode::process() {
    switch (m_step) {
    case Step::baseTime:
        if (gEncoder.getTime(m_baseTime))
            repaint();
        return;
    case Step::initTime:
        if (gEncoder.getTime(m_initTime))
            repaint();
        return;
    case Step::fstopSet:
        if (gEncoder.getInt(m_FStopPartId, 0, sizeof(kFStopPartVarinatns) - 1))
            repaint();
        return;
    case Step::run:
        break;
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && getStepTotalTime(m_currentRun) != kBadTime &&
        gScrollableContent.currentIsPrinted())
        gTimer.start(getPrintTime());

    if (gTimer.justFinished()) {
        if (m_currentRun == 0 && kSubMode == SplitGrade) { // don't take into account base time
            gTimer.reset();
            gBeeper.alarm();
        }
        m_timeTable.setCurrent(++m_currentRun);
        repaint();
    }

    gScrollableContent.scroll(gEncoder.getDir());
    gScrollableContent.paint();
}

void FStopTestMode::repaint() {
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
        m_timeTable.flush();
        gScrollableContent.paint();
        return;
    }
}

Time FStopTestMode::getPrintTime() const {
    uint8_t realStep = m_currentRun;
    if (kSubMode != SplitGrade)
        ++realStep;

    if (realStep == 0)
        return m_baseTime;

    if (realStep == 1)
        return m_initTime;

    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    if (kSubMode == Local)
        return m_initTime * pow(2, (realStep - 1) / stopPart);
    else
        return m_initTime * (pow(2, (realStep - 1) / stopPart) - pow(2, (realStep - 2) / stopPart));
}

Time FStopTestMode::getStepTotalTime(uint8_t id) const {
    uint8_t realId = id;
    if (kSubMode != SplitGrade)
        ++realId;

    if (realId == 0)
        return m_baseTime;

    float stopPart = kFStopPartVarinatns[m_FStopPartId];
    return m_initTime * pow(2, (realId - 1) / stopPart);
}

void FStopTestMode::reset() {
    m_currentRun = 0;
    m_timeTable.setCurrent(0, kSubMode == SplitGrade ? "ntf" : nullptr);
    repaint();
}

void FStopTestMode::setTimeTable() {
    m_timeTable.reset();
    m_timeTable.setPrefix("Run");
    uint8_t id = 0;
    while (true) {
        if (id == m_timeTable.capacity())
            break;
        Time time = getStepTotalTime(id);
        if (time == kBadTime)
            break;
        m_timeTable.setTime(id++, time);
    }
}

const char* FStopTestMode::preview() const {
    if (kSubMode == SplitGrade)
        return "Splt F Stop test";
    if (kSubMode == Local)
        return "Locl F Stop test";
    return "F Stop test";
}

