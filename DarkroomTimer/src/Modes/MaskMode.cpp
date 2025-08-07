#include "MaskMode.h"

#include "../Hardware.h"
#include "../Utils.h"

namespace {

constexpr const char* kSetPrefixes[] = { "S F1", "S F2" };

constexpr const char* kRunPrefixes[] = { "R F1", "R F2" };

} // namespace

MaskMode::MaskMode(uint8_t filterNum) {
    m_filterNum = filterNum;
    m_step = Step::setNum;
    m_notifyMask = 0;
    m_currentMask = 0;
    m_currentFilter = 0;

    auto tableCacheSize = sizeof(gModesCache) / filterNum;
    for (uint8_t filter = 0; filter != filterNum; ++filter) {
        auto& timeTable = m_timeTable[filter];
        timeTable.setBuffer(gModesCache + filter * tableCacheSize, tableCacheSize);
        m_numberOfMasks[filter] = 1;

        timeTable.printBadAsZero(true);
        timeTable.reset();
        timeTable.setTime(0, 8_s);
        for (uint8_t i = 1; i < timeTable.capacity(); ++i)
            timeTable.setTime(i, kBadTime);
    }

    if (filterNum == 1)
        m_maxMasksNumber = min(15, m_timeTable[0].capacity() - 1);
    else
        m_maxMasksNumber = min(7, m_timeTable[0].capacity() - 1);
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
            m_notifyMask &= ~((~0ULL) << m_numberOfMasks[0]);

        for (uint8_t filter = 0; filter != m_filterNum; ++filter) {
            auto numberOfMasks = m_numberOfMasks[filter];
            auto& timeTable = m_timeTable[filter];
            for (uint8_t i = numberOfMasks; i < m_maxMasksNumber; ++i)
                timeTable.setTime(i + 1, kBadTime);

            timeTable.resize(numberOfMasks + 1);
        }
    }

    m_step = ADD_TO_ENUM(Step, m_step, 1);

    setCurrentMask(0, 0);
}

void MaskMode::setCurrentMask(uint8_t filter, uint8_t mask) {
    for (auto& timeTable : m_timeTable)
        timeTable.setCurrent(-1);

    if (filter != m_filterNum)
        m_timeTable[filter].setCurrent(mask, ((m_notifyMask & (1 << mask)) ? "ntf" : nullptr));

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
        // table capacities are equal
        if (gEncoder.getInt(m_numberOfMasks[m_currentFilter], 0, m_maxMasksNumber))
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
    if (gEncoderBtn.pressing()) {
        if (int8_t dir = gEncoder.getDir()) {
            gEncoderBtn.skipEvents();
            moveCurrentMask(dir);
        }

        return;
    }

    if (gEncoderBtn.click()) {
        // let's guess unknown masks
        Time prevTime = m_timeTable[m_currentFilter].getTime(m_currentMask);
        moveCurrentMask(1);
        if (m_timeTable[m_currentFilter].getTime(m_currentMask) == kBadTime)
            m_timeTable[m_currentFilter].setTime(m_currentMask, prevTime);

        repaint();
    }

    auto time = m_timeTable[m_currentFilter].getTime(m_currentMask);
    if (gEncoder.getTime(time)) {
        m_timeTable[m_currentFilter].setTime(m_currentMask, time);
        setCurrentMask(m_currentFilter, m_currentMask);
        gDisplay.resetBlink(false);
    }

    if (m_filterNum == 1 && gStartBtn.click()) {
        m_notifyMask ^= 1 << m_currentMask;
        setCurrentMask(m_currentFilter, m_currentMask);
    }
}

void MaskMode::processRun() {
    if (gTimer.state() == Timer::STOPPED && gStartBtn.click() && m_currentFilter != m_filterNum) {
        auto time = m_timeTable[m_currentFilter].getTime(m_currentMask);
        if (time == kBadTime)
            time = 0_s;

        gTimer.start(time);
    }

    if (gTimer.justFinished()) {
        if (m_notifyMask & (1 << m_currentMask))
            gBeeper.alarm();

        moveCurrentMask(1);

        if (m_currentMask == 0 && m_currentFilter != m_filterNum)
            gBeeper.alarm();
    }

    if (m_currentFilter != m_filterNum)
        gScrollableContent.scroll(gEncoder.getDir());

    gScrollableContent.paint();
}

void MaskMode::repaint() {
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
            m_timeTable[0].setPrefix("Set");
            m_timeTable[0].flush();
        } else {
            for (uint8_t filter = 0; filter != m_filterNum; ++filter) {
                m_timeTable[filter].setPrefix(kSetPrefixes[filter]);
                m_timeTable[filter].flush();
            }
        }
        gScrollableContent.paint();
        return;
    case Step::run:
        gScrollableContent.reset();
        if (m_filterNum == 1) {
            m_timeTable[0].setPrefix("Run");
            m_timeTable[0].flush();
        } else {
            for (uint8_t filter = 0; filter != m_filterNum; ++filter) {
                m_timeTable[filter].setPrefix(kRunPrefixes[filter]);
                m_timeTable[filter].flush();
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
