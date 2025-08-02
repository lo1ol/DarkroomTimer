#include "LinearTestMode.h"

#include "../Tools.h"

LinearTestMode::LinearTestMode(SubMode subMode) : kSubMode(subMode) {
    m_timeTable.setBuffer(gModesCache, sizeof(gModesCache));
    m_timeTable.printBadAsZero(false);

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
        m_timeTable.setCurrent(0, kSubMode == SplitGrade ? "ntf" : nullptr);
    }

    gTimer.reset();

    repaint();
}

void LinearTestMode::process() {
    switch (m_step) {
    case Step::baseTime:
        if (gEncoder.getTime(m_baseTime))
            repaint();
        return;
    case Step::initTime:
        if (gEncoder.getTime(m_initTime))
            repaint();
        return;
    case Step::stepTime:
        if (gEncoder.getTime(m_stepTime))
            repaint();
        return;
    case Step::run:
        break;
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && getTotalTime(m_currentRun) != kBadTime &&
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

void LinearTestMode::repaint() {
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
        m_timeTable.flush();
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
    m_timeTable.setCurrent(0, kSubMode == SplitGrade ? "ntf" : nullptr);
    repaint();
}

void LinearTestMode::setTimeTable() {
    m_timeTable.reset();
    m_timeTable.setPrefix("Run");
    uint8_t id = 0;
    while (true) {
        if (id == m_timeTable.capacity())
            break;
        Time time = getTotalTime(id);
        if (time == kBadTime)
            break;
        m_timeTable.setTime(id++, time);
    }
}

const char* LinearTestMode::preview() const {
    if (kSubMode == SplitGrade)
        return "Splt linear test";
    if (kSubMode == Local)
        return "Locl linear test";
    return "Linear test";
}
