#include "LinearTestMode.h"

#include "../Tools.h"

LinearTestMode::LinearTestMode(SubMode subMode) : kSubMode(subMode) {
    gTimeTable[0].printBadAsZero(false);

    m_baseTime = 2_s;
    m_initTime = 8_s;
    m_stepTime = 2_s;
    m_step = subMode == SplitGrade ? Step::baseTime : Step::initTime;
    m_currentRun = subMode == SplitGrade ? 0 : 1;

    repaint();
}

void LinearTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    if (m_step == Step::baseTime && kSubMode != SplitGrade)
        m_step = Step::initTime;

    m_currentRun = 0;
    if (m_step == Step::run) {
        setTimeTable();
        gTimeTable[0].setCurrent(0, kSubMode == SplitGrade ? "ntf" : nullptr);
    }

    gTimer.reset();

    repaint();
}

void LinearTestMode::process() {
    switch (m_step) {
    case Step::baseTime:
        if (getTime(m_baseTime))
            repaint();
        return;
    case Step::initTime:
        if (getTime(m_initTime))
            repaint();
        return;
    case Step::stepTime:
        if (getTime(m_stepTime))
            repaint();
        return;
    case Step::run:
        break;
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && getTotalTime(m_currentRun) != kBadTime &&
        gScrollableContent.currentIsPrinted())
        gTimer.start(getPrintTime());

    if (gTimer.stopped()) {
        if (m_currentRun == 0 && kSubMode == SplitGrade) { // don't take into account base time
            gTimer.reset();
            gBeeper.alarm();
        }
        gTimeTable[0].setCurrent(++m_currentRun);
        repaint();
    }

    if (gTimer.state() == Timer::STOPPED)
        gScrollableContent.scroll();

    gScrollableContent.paint();
}

void LinearTestMode::repaint() const {
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
    case Step::stepTime:
        gDisplay[0] << preview();
        gDisplay[1] << "Step t:" << m_stepTime;
        return;
    case Step::run:
        gScrollableContent.reset();
        gTimeTable[0].flush(true);
        gScrollableContent.paint();
        break;
    }
}

Time LinearTestMode::getPrintTime() const {
    uint8_t realStep = m_currentRun;
    if (kSubMode != SplitGrade)
        ++realStep;

    if (realStep == 0)
        return m_baseTime;

    if (realStep == 1)
        return m_initTime;

    if (kSubMode == Local)
        return m_initTime + m_stepTime * (realStep - 1);
    else
        return m_stepTime;
}

Time LinearTestMode::getTotalTime(uint8_t id) const {
    uint8_t realId = id;
    if (kSubMode != SplitGrade)
        ++realId;

    if (realId == 0)
        return m_baseTime;

    int32_t res = static_cast<int16_t>(m_stepTime);
    res *= realId - 1;
    res += static_cast<int16_t>(m_initTime);

    return Time{ res };
}
void LinearTestMode::reset() {
    m_currentRun = 0;
    gTimeTable[0].setCurrent(0, kSubMode == SplitGrade ? "ntf" : nullptr);
    repaint();
}

void LinearTestMode::setTimeTable() const {
    gTimeTable[0].empty();
    gTimeTable[0].setPrefix("Run ");
    uint8_t id = 0;
    while (true) {
        if (id == TimeTable::kTimeTableSize)
            break;
        Time time = getTotalTime(id);
        if (time == kBadTime)
            break;
        gTimeTable[0].setTime(id++, time);
    }
}

const char* LinearTestMode::preview() const {
    if (kSubMode == SplitGrade)
        return "Splt linear test";
    if (kSubMode == Local)
        return "Locl linear test";
    return "Linear test";
}
