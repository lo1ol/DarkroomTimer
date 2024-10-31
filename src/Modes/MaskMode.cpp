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

    if (m_step != Step::setMasks) {
        m_step = ADD_TO_ENUM(Step, m_step, 1);

        m_notifyMask &= ~((~0) << m_numberOfMasks);
        for (uint8_t i = m_numberOfMasks; i != TimeTable::kTimeTableSize; ++i)
            gTimeTable.setTime(i, kBadTime);

        gTimeTable.resize(m_numberOfMasks);

        setCurrentMask(0);

        repaint();
        return;
    }

    if (m_currentMask + 1 == m_numberOfMasks) {
        m_step = Step::run;
        setCurrentMask(0);
        repaint();
        return;
    }

    // let's guess unknown masks
    gTimeTable.setTime(m_currentMask + 1, gTimeTable.getTime(m_currentMask));
    setCurrentMask(m_currentMask + 1);
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
    if (gStartBtn.click()) {
        m_notifyMask ^= 1 << m_currentMask;
        setCurrentMask(m_currentMask);
        gDisplay.resetBlink(true);
    }

    auto time = gTimeTable.getTime(m_currentMask);
    if (getTime(time)) {
        gTimeTable.setTime(m_currentMask, time);
        gDisplay.resetBlink();
    }

    gTimeTable.paint();
}

void MaskMode::processRun() {
    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentMask < m_numberOfMasks)
        gTimer.start(gTimeTable.getTime(m_currentMask));

    if (gTimer.stopped()) {
        if (m_notifyMask & (1 << m_currentMask))
            gBeeper.alarm("Notification");

        setCurrentMask(1 + m_currentMask);
    }

    gTimeTable.paint();
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
        gTimeTable.forcePaint();
        return;
    case Step::run:
        gTimeTable.setPrefix("Run ");
        gTimeTable.forcePaint();
        if (m_currentMask == m_numberOfMasks)
            gDisplay[DISPLAY_ROWS - 1] >> "Finished";
        return;
    }
}

void MaskMode::reset() {
    setCurrentMask(0);
}
