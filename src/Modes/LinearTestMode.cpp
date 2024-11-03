#include "LinearTestMode.h"

#include "../Tools.h"

LinearTestMode::LinearTestMode(bool splitGrade) : kSplit(splitGrade) {
    gTimeTable.printBadAsZero(false);

    m_baseTime = 2_s;
    m_initTime = 8_s;
    m_stepTime = 2_s;
    m_step = kSplit ? Step::baseTime : Step::initTime;
    m_currentRun = kSplit ? 0 : 1;

    repaint();
}

void LinearTestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);
    if (m_step == Step::baseTime && !kSplit)
        m_step = Step::initTime;

    m_currentRun = 0;
    if (m_step == Step::run) {
        setTimeTable();
        gTimeTable.setCurrent(0, kSplit ? "ntf" : nullptr);
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
        if (m_currentRun == 0 && kSplit) { // don't take into account base time
            gTimer.reset();
            gBeeper.alarm("Change filter");
        }
        gTimeTable.setCurrent(++m_currentRun);
        gTimeTable.flush();
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
        gTimeTable.flush(true);
        gScrollableContent.paint();
        break;
    }
}

Time LinearTestMode::getPrintTime() const {
    uint8_t realStep = m_currentRun;
    if (!kSplit)
        ++realStep;

    if (realStep == 0)
        return m_baseTime;

    if (realStep == 1)
        return m_initTime;

    return m_stepTime;
}

Time LinearTestMode::getTotalTime(uint8_t id) const {
    uint8_t realId = id;
    if (!kSplit)
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
    gTimeTable.setCurrent(0, kSplit ? "ntf" : nullptr);
    repaint();
}

void LinearTestMode::setTimeTable() const {
    gTimeTable.empty();
    gTimeTable.setPrefix("Run ");
    uint8_t id = 0;
    while (true) {
        if (id == TimeTable::kTimeTableSize)
            break;
        Time time = getTotalTime(id);
        if (time == kBadTime)
            break;
        gTimeTable.setTime(id++, time);
    }
}

const char* LinearTestMode::preview() const {
    if (kSplit)
        return "Splt linear test";
    return "Linear test";
}
