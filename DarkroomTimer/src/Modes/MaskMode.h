#pragma once

#include "AbstractMaskMode.h"

#include "../TimeTable.h"

class MaskMode final : public AbstractMaskMode {
public:
    explicit MaskMode(uint8_t filterNum);

protected:
    void onUpdateTimeTableNums() override;

    void setCurrentMask_(uint8_t filter, uint8_t mask, bool needGuessMask) override;

    bool handleSetCurrentTime() override;
    bool supportNotifyMasks() const override { return true; }

    bool handleEncoderClickAtRun() override { return false; }

    Time getStepTime() const override;

    void printHeaderForSetNum() const override;
    void flushTimeTables() override;
    void flashTimeTablesAtRun() override;

private:
    TimeTable m_timeTable[2];
};
