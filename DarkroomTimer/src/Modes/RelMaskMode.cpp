#include "RelMaskMode.h"

#include "../Hardware.h"
#include "../Utils.h"

namespace {

constexpr const char* kSetPrefixes[] = { "1" kSheetSym, "2" kSheetSym };
constexpr const char* kRunPrefixes[] = { "1" kPlaySym, "2" kPlaySym };

} // namespace

RelMaskMode::RelMaskMode(uint8_t filterNum) : AbstractMaskMode(filterNum) {
    auto tableCacheSize = sizeof(gModesCache) / kFilterNum;
    for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
        auto& relTimeTable = m_relTimeTable[filter];
        m_numberOfMasks[filter] = 1;
        relTimeTable.setBuffer(gModesCache + filter * tableCacheSize, tableCacheSize);
        relTimeTable.reset();
        relTimeTable.setBaseTime(8_s);
        for (uint8_t i = 0; i != relTimeTable.capacity(); ++i)
            relTimeTable.setRelTime(i, RelTime());
    }

    assert(kMaxMasksNumber <= m_relTimeTable[0].capacity());
}

void RelMaskMode::onUpdateTimeTableNums() {
    for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
        auto numberOfMasks = m_numberOfMasks[filter];
        auto& relTimeTable = m_relTimeTable[filter];
        for (uint8_t i = numberOfMasks; i < kMaxMasksNumber; ++i)
            relTimeTable.setRelTime(i, RelTime());

        relTimeTable.resize(numberOfMasks);
        relTimeTable.setSecView(false);
    }
}

void RelMaskMode::setCurrentMask_(uint8_t filter, uint8_t mask, [[maybe_unused]] bool needGuessMask) {
    for (auto& relTimeTable : m_relTimeTable)
        relTimeTable.setCurrent(-2);

    if (filter == kFilterNum)
        return;

    m_relTimeTable[filter].setCurrent(mask - 1);
}

bool RelMaskMode::handleSetCurrentTime() {
    if (m_currentMask == 0) {
        auto time = m_relTimeTable[m_currentFilter].getBaseTime();
        if (!gEncoder.getTime(time))
            return false;

        m_relTimeTable[m_currentFilter].setBaseTime(time);
        return true;
    }

    auto relTime = m_relTimeTable[m_currentFilter].getRelTime(m_currentMask - 1);
    if (!gEncoder.getRelTime(relTime))
        return false;

    m_relTimeTable[m_currentFilter].setRelTime(m_currentMask - 1, relTime);
    return true;
}

bool RelMaskMode::handleEncoderClickAtRun() {
    for (auto& relTimeTable : m_relTimeTable)
        relTimeTable.toggleSecView();

    return true;
}

Time RelMaskMode::getStepTime() const {
    Time t = m_relTimeTable[m_currentFilter].getTime(m_currentMask - 1);
    return (t == kBadTime) ? 0_s : t;
}

void RelMaskMode::printHeaderForSetNum() const {
    if (kFilterNum == 1)
        gDisplay[0] << F("Rel mask print");
    else
        gDisplay[0] << F("Filter ") << m_currentFilter + 1;
}

void RelMaskMode::flushTimeTables() {
    if (kFilterNum == 1) {
        m_relTimeTable[0].setPrefix(kSheetSym);
        m_relTimeTable[0].flush();
    } else {
        for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
            m_relTimeTable[filter].setPrefix(kSetPrefixes[filter]);
            m_relTimeTable[filter].flush();
        }
    }
}

void RelMaskMode::flashTimeTablesAtRun() {
    if (kFilterNum == 1) {
        m_relTimeTable[0].setPrefix(kPlaySym);
        m_relTimeTable[0].flush();
    } else {
        for (uint8_t filter = 0; filter != kFilterNum; ++filter) {
            m_relTimeTable[filter].setPrefix(kRunPrefixes[filter]);
            m_relTimeTable[filter].flush();
        }
    }
}
