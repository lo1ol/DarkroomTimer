#include "RelMaskMode.h"

#include "../Tools.h"

namespace {

constexpr const char* kSetPrefixes[] = { "S F1", "S F2" };

constexpr const char* kRunPrefixes[] = { "R F1", "R F2" };

} // namespace

RelMaskMode::RelMaskMode(uint8_t filterNum) {
    m_filterNum = filterNum;

    m_step = Step::setNum;
    m_numberOfMasks[0] = 1;
    m_numberOfMasks[1] = 1;
    m_currentMask = -1;
    m_currentFilter = 0;

    for (auto& relTimeTable : gRelTimeTable) {
        relTimeTable.reset();
        relTimeTable.setBaseTime(8_s);
        for (uint8_t i = 0; i != RelTimeTable::kTableSize; ++i)
            relTimeTable.setRelTime(i, RelTime());
    }

    repaint();
}

void RelMaskMode::switchMode() {
    gTimer.reset();

    if (m_step == Step::setNum && m_currentFilter + 1 < m_filterNum) {
        ++m_currentFilter;
        m_step = Step::setNum;
        repaint();
        return;
    }

    if (m_step == Step::setNum) {
        for (uint8_t filter = 0; filter != m_filterNum; ++filter) {
            for (uint8_t i = m_numberOfMasks[filter]; i != m_numberOfMasks[filter]; ++i)
                gRelTimeTable[filter].setRelTime(i, RelTime());

            gRelTimeTable[filter].resize(m_numberOfMasks[filter]);
            gRelTimeTable[filter].setSecView(false);
        }
    }

    m_step = ADD_TO_ENUM(Step, m_step, 1);

    setCurrentMask(0, -1);
}

void RelMaskMode::setCurrentMask(uint8_t filter, uint8_t mask) {
    if (filter == m_filterNum) {
        for (auto& relTimeTable : gRelTimeTable)
            relTimeTable.setCurrent(-2);
    } else {
        gRelTimeTable[m_currentFilter].setCurrent(-2);
        gRelTimeTable[filter].setCurrent(mask);
    }

    m_currentFilter = filter;
    m_currentMask = mask;

    repaint();
}

void RelMaskMode::moveCurrentMask(int8_t dir) {
    auto newFilter = m_currentFilter;
    auto newMask = m_currentMask;
    if (m_currentMask == -1 && dir == -1) {
        if (m_currentFilter == 0)
            newFilter = m_filterNum - 1;
        else
            --newFilter;
        newMask = m_numberOfMasks[newFilter] - 1;
    } else if (m_currentMask == m_numberOfMasks[m_currentFilter] - 1 && dir == 1) {
        newMask = -1;
        if (m_currentFilter + 1 == m_filterNum)
            newFilter = m_step == Step::run ? m_filterNum : 0;
        else
            ++newFilter;
    } else {
        newMask += dir;
    }

    setCurrentMask(newFilter, newMask);
}

void RelMaskMode::process() {
    switch (m_step) {
    case Step::setNum:
        if (getInt(m_numberOfMasks[m_currentFilter], 0, RelTimeTable::kTableSize))
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

void RelMaskMode::processSetMasks() {
    if (gExtraBtn.pressing()) {
        if (int8_t dir = getEncoderDir()) {
            gExtraBtn.skipEvents();
            gDisplay.resetBlink(true);
            moveCurrentMask(dir);
        }

        return;
    }

    if (gExtraBtn.click()) {
        moveCurrentMask(1);
        gDisplay.resetBlink(true);
    }

    bool changed = false;
    if (m_currentMask == -1) {
        auto time = gRelTimeTable[m_currentFilter].getBaseTime();
        changed = getTime(time);
        if (changed)
            gRelTimeTable[m_currentFilter].setBaseTime(time);
    } else {
        auto relTime = gRelTimeTable[m_currentFilter].getRelTime(m_currentMask);
        changed = getRelTime(relTime);
        if (changed)
            gRelTimeTable[m_currentFilter].setRelTime(m_currentMask, relTime);
    }

    if (changed) {
        setCurrentMask(m_currentFilter, m_currentMask);
        gDisplay.resetBlink();
    }
}

void RelMaskMode::processRun() {
    if (gExtraBtn.click()) {
        for (auto& relTimeTable : gRelTimeTable)
            relTimeTable.toggleSecView();
        repaint();
    }

    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentFilter != m_filterNum)
        gTimer.start(getStepTime());

    if (gTimer.stopped()) {
        moveCurrentMask(1);
        if (m_currentMask == -1 && m_currentFilter != m_filterNum)
            gBeeper.alarm();
    }

    if (gTimer.state() == Timer::STOPPED && m_currentFilter != m_filterNum)
        gScrollableContent.scroll();

    gScrollableContent.paint();
}

Time RelMaskMode::getStepTime() {
    return gRelTimeTable[m_currentFilter].getTime(m_currentMask);
}

void RelMaskMode::repaint() const {
    gDisplay.reset();

    switch (m_step) {
    case Step::setNum:
        if (m_filterNum == 1)
            gDisplay[0] << "Rel mask print";
        else
            gDisplay[0] << "Filter " << m_currentFilter + 1;

        gDisplay[1] << "Mask num: " << m_numberOfMasks[m_currentFilter];
        return;
    case Step::setMasks:
        gScrollableContent.reset();
        if (m_filterNum == 1) {
            gRelTimeTable[0].setPrefix("Set");
            gRelTimeTable[0].flush(true);
        } else {
            for (uint8_t filter = 0; filter != m_filterNum; ++filter) {
                gRelTimeTable[filter].setPrefix(kSetPrefixes[filter]);
                gRelTimeTable[filter].flush(true);
            }
        }
        gScrollableContent.paint();
        return;
    case Step::run:
        gScrollableContent.reset();
        if (m_filterNum == 1) {
            gRelTimeTable[0].setPrefix("Run");
            gRelTimeTable[0].flush(true);
        } else {
            for (uint8_t filter = 0; filter != m_filterNum; ++filter) {
                gRelTimeTable[filter].setPrefix(kRunPrefixes[filter]);
                gRelTimeTable[filter].flush(true);
            }
        }

        gScrollableContent.paint();
        if (m_currentFilter == m_filterNum)
            gDisplay[DISPLAY_ROWS - 1] >> "Finished";
        return;
    }
}

void RelMaskMode::reset() {
    if (m_step != Step::setMasks)
        setCurrentMask(0, -1);
}
