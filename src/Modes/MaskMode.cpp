#include "MaskMode.h"

#include "../Tools.h"

MaskMode::MaskMode() {
    gTimeTable.empty();
    gTimeTable.printBadAsZero(true);

    m_numberOfMasks = 3;
    gTimeTable.setTime(0, 8_s);
    m_step = Step::setNum;
    m_currentMask = 0;
    m_notifyMask = 0;

    for (uint8_t i = 1; i != TimeTable::kTimeTableSize; ++i)
        gTimeTable.setTime(i, kBadTime);

    repaint();
}

void MaskMode::switchMode() {
    gTimer.reset();

    if (m_step == Step::setNum) {
        m_notifyMask &= ~((~0) << m_numberOfMasks);
        for (uint8_t i = m_numberOfMasks; i != TimeTable::kTimeTableSize; ++i)
            gTimeTable.setTime(i, kBadTime);

        gTimeTable.resize(m_numberOfMasks);
    }

    m_step = ADD_TO_ENUM(Step, m_step, 1);
    setCurrentMask(0);
    repaint();
}

void MaskMode::setCurrentMask(uint8_t id) {
    m_currentMask = id;
    if (id == m_numberOfMasks)
        gTimeTable.setCurrent(-1);
    else
        gTimeTable.setCurrent(id, ((m_notifyMask & (1 << id)) ? "ntf" : nullptr));

    repaint();
}

void MaskMode::process() {
    switch (m_step) {
    case Step::setNum:
        if (getInt(m_numberOfMasks, 2, TimeTable::kTimeTableSize))
            repaint();
        return;
    case Step::setMasks:
        processSetMasks();
        return;
    case Step::run:
        processRun();
        return;
    }
}

void MaskMode::processSetMasks() {
    if (gExtraBtn.pressing()) {
        uint8_t currentMask = m_currentMask;
        if (getInt(currentMask, 0, m_numberOfMasks - 1)) {
            gExtraBtn.skipEvents();
            gDisplay.resetBlink(true);
            setCurrentMask(currentMask);
        }

        return;
    }

    if (gExtraBtn.click()) {
        uint8_t newMask = m_currentMask + 1;
        if (newMask == m_numberOfMasks)
            newMask = 0;
        // let's guess unknown masks
        if (gTimeTable.getTime(newMask) == kBadTime)
            gTimeTable.setTime(newMask, gTimeTable.getTime(m_currentMask));

        setCurrentMask(newMask);
        gDisplay.resetBlink(true);
    }

    if (gStartBtn.click()) {
        m_notifyMask ^= 1 << m_currentMask;
        setCurrentMask(m_currentMask);
        gDisplay.resetBlink(true);
    }

    auto time = gTimeTable.getTime(m_currentMask);
    if (getTime(time)) {
        gTimeTable.setTime(m_currentMask, time);
        setCurrentMask(m_currentMask);
        gDisplay.resetBlink();
    }
}

void MaskMode::processRun() {
    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentMask < m_numberOfMasks) {
        auto time = gTimeTable.getTime(m_currentMask);
        if (time == kBadTime)
            time = 0_s;

        gTimer.start(time);
    }

    if (gTimer.stopped()) {
        if (m_notifyMask & (1 << m_currentMask))
            gBeeper.alarm("Notification");

        setCurrentMask(1 + m_currentMask);
    }

    if (gTimer.state() == Timer::STOPPED && m_currentMask != m_numberOfMasks)
        gScrollableContent.scroll();

    gScrollableContent.paint();
}

void MaskMode::repaint() const {
    gDisplay.reset();

    switch (m_step) {
    case Step::setNum:
        gDisplay[0] << preview();
        gDisplay[1] << "Mask num: " << m_numberOfMasks;
        return;
    case Step::setMasks:
        gTimeTable.setPrefix("Set ");
        gTimeTable.flush(true);
        gScrollableContent.paint();
        return;
    case Step::run:
        gTimeTable.setPrefix("Run ");
        gTimeTable.flush(true);
        gScrollableContent.paint();
        if (m_currentMask == m_numberOfMasks)
            gDisplay[DISPLAY_ROWS - 1] >> "Finished";
        return;
    }
}

void MaskMode::reset() {
    if (m_step == Step::run)
        setCurrentMask(0);
}
