#include "TestMode.h"

#include "../Hardware.h"
#include "../Utils.h"

TestMode::TestMode(SubMode subMode) : kSubMode(subMode) {
    m_timeTable.setBuffer(gModesCache, sizeof(gModesCache));
    m_timeTable.printBadAsZero(false);

    m_preflashTime = 0_s;
    m_step = Step::initTime;
    m_currentRun = 0;
    m_localized = false;

    if (kSubMode == Local)
        m_localized = true;

    if (kSubMode == SplitGrade)
        m_preflashTime = 2_s;

    if (kSubMode == Expert)
        m_step = Step::setLocalized;

    else if (kSubMode == SplitGrade)
        m_step = Step::preflashTime;
}

void TestMode::switchMode() {
    m_step = ADD_TO_ENUM(Step, m_step, 1);

    if (m_step == Step::setLocalized && kSubMode != Expert)
        m_step = Step::preflashTime;

    if (m_step == Step::preflashTime && kSubMode != SplitGrade && kSubMode != Expert)
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
    case Step::setLocalized:
        if (gEncoder.getBool(m_localized))
            repaint();
        return;
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
        if (m_currentRun == 0 && hasPreflashTime())
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
    case Step::setLocalized:
        gDisplay[0] << header();
        gDisplay[1] << F("Local tests:") << (m_localized ? "on" : "off");
        return;
    case Step::preflashTime:
        gDisplay[0] << header();
        if (kSubMode == SplitGrade)
            gDisplay[1] << F("First t:");
        else
            gDisplay[1] << F("Preflash t:");

        gDisplay[1] << m_preflashTime;
        return;
    case Step::initTime:
        gDisplay[0] << header();
        gDisplay[1] << F("Init t:") << m_initTime;
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
    if (!hasPreflashTime())
        ++realId;

    if (realId == 0)
        return m_preflashTime;

    if (realId == 1)
        return m_initTime;

    if (m_localized)
        return getStepTotalTime_(realId - 1);
    return getStepTime_(realId - 1);
}

Time TestMode::getStepTotalTime(uint8_t id) const {
    uint8_t realId = id;
    if (!hasPreflashTime())
        ++realId;

    if (realId == 0)
        return m_preflashTime;

    if (realId == 1)
        return m_initTime;

    return getStepTotalTime_(realId - 1);
}

bool TestMode::hasPreflashTime() const {
    return kSubMode == SplitGrade || m_preflashTime;
}

void TestMode::reset() {
    m_currentRun = 0;
    m_timeTable.setCurrent(0, hasPreflashTime() ? "ntf" : nullptr);
}

void TestMode::setTimeTable() {
    m_timeTable.reset();
    m_timeTable.setPrefix(kPlaySym);
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
