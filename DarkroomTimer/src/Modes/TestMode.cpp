#include "TestMode.h"

#include "../Hardware.h"
#include "../Utils.h"

TestMode::TestMode(SubMode subMode) : kSubMode(subMode) {
    m_timeTable.setBuffer(gModesCache, sizeof(gModesCache));
    m_timeTable.printBadAsZero(false);

    m_preflashTime = 0_s;
    m_step = Step::initTime;
    m_currentRun = 0;

    if (kSubMode == SplitGrade)
        m_preflashTime = 2_s;

    if (kSubMode == SplitGrade)
        m_step = Step::preflashTime;
}

void TestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);

    if (m_step == Step::preflashTime && kSubMode != SplitGrade)
        m_step = Step::initTime;

    if (m_step == Step::run) {
        setTimeTable();
        reset();
    }

    gTimer.reset();

    repaint();
}

void TestMode::process() {
    switch (m_step) {
    case Step::preflashTime:
        if (gEncoder.getTime(m_preflashTime))
            repaint();
        return;
    case Step::initTime:
        if (gEncoder.getTime(m_initTime))
            repaint();
        return;
    case Step::setStep:
        if (handleStepSet())
            repaint();
        return;
    case Step::run:
        break;
    case Step::last_:
        assert(false);
        return;
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && getStepTotalTime(m_currentRun) != kBadTime &&
        gScrollableContent.hasCurrent())
        gTimer.start(getPrintTime());

    if (gTimer.justFinished()) {
        if (m_currentRun == 0 && kSubMode == SplitGrade)
            gBeeper.alarm();

        m_timeTable.setCurrent(++m_currentRun);
        repaint();
    }

    gScrollableContent.scroll(gEncoder.getDir());
    gScrollableContent.paint();
}

void TestMode::repaint() {
    gDisplay.reset();

    switch (m_step) {
    case Step::preflashTime:
        gDisplay[0] << header();
        gDisplay[1] << "First t:" << m_preflashTime;
        return;
    case Step::initTime:
        gDisplay[0] << header();
        gDisplay[1] << "Init t:" << m_initTime;
        return;
    case Step::setStep:
        gDisplay[0] << header();
        printStep();
        return;
    case Step::run:
        gScrollableContent.reset();
        m_timeTable.flush();
        gScrollableContent.paint();
        return;
    case Step::last_:
        assert(false);
        return;
    }
}

Time TestMode::getPrintTime() const {
    uint8_t realId = m_currentRun;
    if (kSubMode != SplitGrade)
        ++realId;

    if (realId == 0)
        return m_preflashTime;

    if (realId == 1)
        return m_initTime;

    if (kSubMode == Local)
        return getStepTotalTime_(realId - 1);
    return getStepTime_(realId - 1);
}

Time TestMode::getStepTotalTime(uint8_t id) const {
    uint8_t realId = id;
    if (kSubMode != SplitGrade)
        ++realId;

    if (realId == 0)
        return m_preflashTime;

    if (realId == 1)
        return m_initTime;

    return getStepTotalTime_(realId - 1);
}

void TestMode::reset() {
    m_currentRun = 0;
    m_timeTable.setCurrent(0, kSubMode == SplitGrade ? "ntf" : nullptr);
}

void TestMode::setTimeTable() {
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
