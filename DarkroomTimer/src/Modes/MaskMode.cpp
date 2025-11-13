#include "MaskMode.h"

#include "../Hardware.h"
#include "../Utils.h"

namespace {

constexpr const char* kSetPrefixes[] = { "S F1", "S F2" };

constexpr const char* kRunPrefixes[] = { "R F1", "R F2" };

} // namespace

MaskMode::MaskMode(uint8_t filterNum) : AbstractMaskMode(filterNum) {
    auto tableCacheSize = sizeof(gModesCache) / kFilterNum;

    for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
        auto& timeTable = m_timeTable[filter];
        timeTable.setBuffer(gModesCache + filter * tableCacheSize, tableCacheSize);
        m_numberOfMasks[filter] = 1;

        timeTable.printBadAsZero(true);
        timeTable.reset();
        timeTable.setTime(0, 8_s);
        for (uint8_t i = 1; i < timeTable.capacity(); ++i)
            timeTable.setTime(i, kBadTime);
    }

    assert(kMaxMasksNumber <= m_timeTable[0].capacity() - 1);
}

void MaskMode::onUpdateTimeTableNums() {
    for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
        auto numberOfMasks = m_numberOfMasks[filter];
        auto& timeTable = m_timeTable[filter];
        for (uint8_t i = numberOfMasks; i < kMaxMasksNumber; ++i)
            timeTable.setTime(i + 1, kBadTime);

        timeTable.resize(numberOfMasks + 1);
    }
}

void MaskMode::setCurrentMask_(uint8_t filter, uint8_t mask, bool needGuessMask) {
    for (auto& timeTable : m_timeTable)
        timeTable.setCurrent(-1);

    if (filter == kFilterNum)
        return;

    if (needGuessMask && m_timeTable[filter].getTime(mask) == kBadTime) {
        Time prevTime = m_timeTable[m_currentFilter].getTime(m_currentMask);
        m_timeTable[filter].setTime(mask, prevTime);
    }

    m_timeTable[filter].setCurrent(mask, ((m_notifyMask & (1 << mask)) ? "ntf" : nullptr));
}

bool MaskMode::handleSetCurrentTime() {
    auto time = m_timeTable[m_currentFilter].getTime(m_currentMask);
    if (!gEncoder.getTime(time))
        return false;

    m_timeTable[m_currentFilter].setTime(m_currentMask, time);
    return true;
}

Time MaskMode::getStepTime() const {
    auto res = m_timeTable[m_currentFilter].getTime(m_currentMask);
    return res == kBadTime ? 0_s : res;
}

void MaskMode::printHeaderForSetNum() const {
    if (kFilterNum == 1)
        gDisplay[0] << F("Mask printing");
    else
        gDisplay[0] << F("Filter ") << m_currentFilter + 1;
}

void MaskMode::flushTimeTables() {
    if (kFilterNum == 1) {
        m_timeTable[0].setPrefix("Set");
        m_timeTable[0].flush();
    } else {
        for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
            m_timeTable[filter].setPrefix(kSetPrefixes[filter]);
            m_timeTable[filter].flush();
        }
    }
}

void MaskMode::flashTimeTablesAtRun() {
    if (kFilterNum == 1) {
        m_timeTable[0].setPrefix("Run");
        m_timeTable[0].flush();
    } else {
        for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
            m_timeTable[filter].setPrefix(kRunPrefixes[filter]);
            m_timeTable[filter].flush();
        }
    }
}
