#include "MaskMode.h"

#include "../Tools.h"

namespace {

constexpr const char* kSetPrefixes[] = { "S F1 ", "S F2 " };

constexpr const char* kRunPrefixes[] = { "R F1 ", "R F2 " };

} // namespace

MaskMode::MaskMode(uint8_t filterNum) {
    m_filterNum = filterNum;

    m_step = Step::setNum;
    m_numberOfMasks[0] = 1;
    m_numberOfMasks[1] = 1;
    m_notifyMask = 0;
    m_currentMask = 0;
    m_currentFilter = 0;

    for (auto& timeTable : gTimeTable) {
        timeTable.printBadAsZero(true);
        timeTable.empty();
        timeTable.setTime(0, 8_s);
        for (uint8_t i = 1; i != TimeTable::kTimeTableSize; ++i)
            timeTable.setTime(i, kBadTime);
    }

    repaint();
}

void MaskMode::switchMode() {
    gTimer.reset();

    if (m_step == Step::setNum && m_currentFilter + 1 < m_filterNum) {
        ++m_currentFilter;
        m_step = Step::setNum;
        repaint();
        return;
    }

    if (m_step == Step::setNum) {
        if (m_filterNum == 1)
            m_notifyMask &= ~((~0) << m_numberOfMasks[0]);

        for (uint8_t filter = 0; filter != m_filterNum; ++filter) {
            for (uint8_t i = m_numberOfMasks[filter] + 1; i != TimeTable::kTimeTableSize; ++i)
                gTimeTable[filter].setTime(i, kBadTime);

            gTimeTable[filter].resize(m_numberOfMasks[filter] + 1);
        }
    }

    m_step = ADD_TO_ENUM(Step, m_step, 1);

    setCurrentMask(0, 0);
}

void MaskMode::setCurrentMask(uint8_t filter, uint8_t mask) {
    if (filter == m_filterNum) {
        for (auto& timeTable : gTimeTable)
            timeTable.setCurrent(-1);
    } else {
        gTimeTable[m_currentFilter].setCurrent(-1);
        gTimeTable[filter].setCurrent(mask);
        gTimeTable[0].setCurrent(mask, ((m_notifyMask & (1 << mask)) ? "ntf" : nullptr));
    }

    m_currentFilter = filter;
    m_currentMask = mask;

    repaint();
}

void MaskMode::moveCurrentMask(int8_t dir) {
    auto newFilter = m_currentFilter;
    auto newMask = m_currentMask;
    if (m_currentMask == 0 && dir == -1) {
        if (m_currentFilter == 0)
            newFilter = m_filterNum - 1;
        else
            --newFilter;
        newMask = m_numberOfMasks[newFilter];
    } else if (m_currentMask == m_numberOfMasks[m_currentFilter] && dir == 1) {
        newMask = 0;
        if (m_currentFilter + 1 == m_filterNum)
            newFilter = m_step == Step::run ? m_filterNum : 0;
        else
            ++newFilter;
    } else {
        newMask += dir;
    }

    setCurrentMask(newFilter, newMask);
}

void MaskMode::process() {
    switch (m_step) {
    case Step::setNum:
        if (getInt(m_numberOfMasks[m_currentFilter], 0, TimeTable::kTimeTableSize - 1))
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
        if (int8_t dir = getEncoderDir()) {
            gExtraBtn.skipEvents();
            gDisplay.resetBlink(true);
            moveCurrentMask(dir);
        }

        return;
    }

    if (gExtraBtn.click()) {
        // let's guess unknown masks
        Time prevTime = gTimeTable[m_currentFilter].getTime(m_currentMask);
        moveCurrentMask(1);
        if (gTimeTable[m_currentFilter].getTime(m_currentMask) == kBadTime)
            gTimeTable[m_currentFilter].setTime(m_currentMask, prevTime);

        repaint();
        gDisplay.resetBlink(true);
    }

    auto time = gTimeTable[m_currentFilter].getTime(m_currentMask);
    if (getTime(time)) {
        gTimeTable[m_currentFilter].setTime(m_currentMask, time);
        setCurrentMask(m_currentFilter, m_currentMask);
        gDisplay.resetBlink();
    }

    if (m_filterNum == 1 && gStartBtn.click()) {
        m_notifyMask ^= 1 << m_currentMask;
        setCurrentMask(m_currentFilter, m_currentMask);
        gDisplay.resetBlink(true);
    }
}

void MaskMode::processRun() {
    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentFilter != m_filterNum) {
        auto time = gTimeTable[m_currentFilter].getTime(m_currentMask);
        if (time == kBadTime)
            time = 0_s;

        gTimer.start(time);
    }

    if (gTimer.stopped()) {
        if (m_notifyMask & (1 << m_currentMask))
            gBeeper.alarm();

        moveCurrentMask(1);

        if (m_currentMask == 0 && m_currentFilter != m_filterNum)
            gBeeper.alarm();
    }

    if (gTimer.state() == Timer::STOPPED && m_currentFilter != m_filterNum)
        gScrollableContent.scroll();

    gScrollableContent.paint();
}

void MaskMode::repaint() const {
    gDisplay.reset();

    switch (m_step) {
    case Step::setNum:
        if (m_filterNum == 1)
            gDisplay[0] << "Mask printing";
        else
            gDisplay[0] << "Filter " << m_currentFilter + 1;

        gDisplay[1] << "Mask num: " << m_numberOfMasks[m_currentFilter];
        return;
    case Step::setMasks:
        gScrollableContent.reset();
        if (m_filterNum == 1) {
            gTimeTable[0].setPrefix("Set ");
            gTimeTable[0].flush(true);
        } else {
            for (uint8_t filter; filter != m_filterNum; ++filter) {
                gTimeTable[filter].setPrefix(kSetPrefixes[filter]);
                gTimeTable[filter].flush(true);
            }
        }
        gScrollableContent.paint();
        return;
    case Step::run:
        gScrollableContent.reset();
        if (m_filterNum == 1) {
            gTimeTable[0].setPrefix("Run ");
            gTimeTable[0].flush(true);
        } else {
            for (uint8_t filter; filter != m_filterNum; ++filter) {
                gTimeTable[filter].setPrefix(kRunPrefixes[filter]);
                gTimeTable[filter].flush(true);
            }
        }

        gScrollableContent.paint();
        if (m_currentFilter == m_filterNum)
            gDisplay[DISPLAY_ROWS - 1] >> "Finished";
        return;
    }
}

void MaskMode::reset() {
    if (m_step == Step::run)
        setCurrentMask(0, 0);
}
