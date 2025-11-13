#include "AbstractMaskMode.h"

#include "../Hardware.h"
#include "../Utils.h"

AbstractMaskMode::AbstractMaskMode(uint8_t filterNum)
    : kFilterNum(filterNum), kMaxMasksNumber(filterNum == 1 ? 15 : 7) {}

void AbstractMaskMode::switchMode() {
    gTimer.reset();

    if (m_step == Step::setNum && m_currentFilter + 1 < kFilterNum) {
        ++m_currentFilter;
        repaint();
        return;
    }

    if (m_step == Step::setNum) {
        if (kFilterNum == 1 && supportNotifyMasks())
            m_notifyMask &= ~((~0ULL) << m_numberOfMasks[0]);

        onUpdateTimeTableNums();
    }

    m_step = ADD_TO_ENUM(Step, m_step, 1);

    setCurrentMask(0, 0);
}

void AbstractMaskMode::setCurrentMask(uint8_t filter, uint8_t mask, bool needGuessMask) {
    setCurrentMask_(filter, mask, needGuessMask);

    m_currentFilter = filter;
    m_currentMask = mask;

    repaint();
}

void AbstractMaskMode::moveCurrentMask(int8_t dir, bool needGuessMask) {
    auto newFilter = m_currentFilter;
    auto newMask = m_currentMask;
    if (m_currentMask == 0 && dir == -1) {
        if (m_currentFilter == 0)
            newFilter = kFilterNum - 1;
        else
            --newFilter;
        newMask = m_numberOfMasks[newFilter];
    } else if (m_currentMask == m_numberOfMasks[m_currentFilter] && dir == 1) {
        newMask = 0;
        if (m_currentFilter + 1 == kFilterNum)
            newFilter = m_step == Step::run ? kFilterNum : 0;
        else
            ++newFilter;
    } else {
        newMask += dir;
    }

    setCurrentMask(newFilter, newMask, needGuessMask);
}

void AbstractMaskMode::process() {
    switch (m_step) {
    case Step::setNum:
        if (gEncoder.getInt(m_numberOfMasks[m_currentFilter], 0, kMaxMasksNumber))
            repaint();
        return;
    case Step::setMasks:
        processSetMasks();
        return;
    case Step::run:
        processRun();
        return;
    case Step::last_:
        assert(false);
        return;
    }
}

void AbstractMaskMode::processSetMasks() {
    if (gEncoderBtn.pressing()) {
        if (int8_t dir = gEncoder.getDir()) {
            gEncoderBtn.skipEvents();
            moveCurrentMask(dir);
        }

        return;
    }

    if (gEncoderBtn.click())
        moveCurrentMask(1, true);

    if (handleSetCurrentTime()) {
        setCurrentMask(m_currentFilter, m_currentMask);
        gDisplay.resetBlink(false);
    }

    if (kFilterNum == 1 && supportNotifyMasks() && gStartBtn.click()) {
        m_notifyMask ^= 1 << m_currentMask;
        setCurrentMask(m_currentFilter, m_currentMask);
        gDisplay.resetBlink(true);
    }
}

void AbstractMaskMode::processRun() {
    if (gEncoderBtn.click() && handleEncoderClickAtRun())
        repaint();

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentFilter != kFilterNum)
        gTimer.start(getStepTime());

    if (gTimer.justFinished()) {
        if (kFilterNum == 1 && m_notifyMask & (1 << m_currentMask))
            gBeeper.alarm();

        moveCurrentMask(1);
        if (m_currentMask == 0 && m_currentFilter != kFilterNum)
            gBeeper.alarm();
    }

    if (m_currentFilter != kFilterNum)
        gScrollableContent.scroll(gEncoder.getDir());

    gScrollableContent.paint();
}

void AbstractMaskMode::repaint() {
    gDisplay.reset();

    switch (m_step) {
    case Step::setNum:
        printHeaderForSetNum();
        gDisplay[1] << F("Mask num: ") << m_numberOfMasks[m_currentFilter];
        return;
    case Step::setMasks:
        gScrollableContent.reset();
        flushTimeTables();
        gScrollableContent.paint();
        return;
    case Step::run:
        gScrollableContent.reset();
        flashTimeTablesAtRun();
        gScrollableContent.paint();
        if (m_currentFilter == kFilterNum)
            gDisplay[DISPLAY_ROWS - 1] >> F("Finished");
        return;
    case Step::last_:
        assert(false);
        return;
    }
}

void AbstractMaskMode::reset() {
    if (m_step != Step::setMasks)
        setCurrentMask(0, 0);
}
